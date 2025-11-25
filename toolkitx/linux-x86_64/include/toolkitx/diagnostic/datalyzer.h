#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

// ============================================================
//  File: datalyzer.h
//  Purpose: 通用数值数据调试 + 分析工具
//  Features:
//      1. 跨语言可读 JSON 数据格式
//      2. 保存/加载通用线性数据
//      3. 文件间数据对比（L2、L1、Cosine 等）
// ============================================================
namespace diagnostic::datalyzer {
    inline std::string trim_copy(std::string s) {
        s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
        return s;
    }

    template<typename T>
    void print(
        const T *data, const size_t count, const std::string &title = "",
        const int per_line = 10, const int width = 14, const int precision = 6
    ) {
        if (!title.empty()) std::cout << "[" << title << "]\n";

        std::cout << std::fixed << std::setprecision(precision);

        const std::string indent = "    "; // 4 spaces, stable alignment
        for (size_t i = 0; i < count; ++i) {
            if (i % per_line == 0) std::cout << indent;

            std::cout << std::setw(width) << static_cast<double>(data[i]);

            if ((i + 1) % per_line == 0) std::cout << "\n";
        }

        if (count % per_line != 0) std::cout << "\n";
    }

    // ------------------------------------------------------------
    // 跨语言可读 JSON 保存
    // ------------------------------------------------------------
    template<typename T>
    void save(const T *data, const size_t count, const std::string &file_path, const int precision = 6) {
        std::ofstream ofs(file_path, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) throw std::runtime_error("cannot open file: " + file_path);

        ofs << std::fixed << std::setprecision(precision);
        ofs << "{";
        ofs << "  \"count\": " << count << ",";
        ofs << "  \"data\": [";

        for (size_t i = 0; i < count; ++i) {
            ofs << static_cast<double>(data[i]);
            if (i + 1 < count) ofs << ", ";
        }

        ofs << "]";
        ofs << "}";
    }

    // ------------------------------------------------------------
    // 加载 JSON（跨语言可读）
    // ------------------------------------------------------------
    template<typename T>
    std::vector<T> load(const std::string &file_path) {
        std::ifstream ifs(file_path);
        if (!ifs.is_open()) throw std::runtime_error("cannot open file: " + file_path);

        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string json = buffer.str();

        size_t pos_data = json.find("\"data\"");
        size_t lb = json.find('[', pos_data);
        size_t rb = json.find(']', lb);

        std::string arr = json.substr(lb + 1, rb - lb - 1);
        std::stringstream ss(arr);

        std::vector<T> result;
        std::string item;

        while (std::getline(ss, item, ',')) {
            item = trim_copy(item);
            if (!item.empty()) result.push_back(static_cast<T>(std::stod(item)));
        }

        return result;
    }

    // ------------------------------------------------------------
    // 对比函数：L1, L2, Cosine
    // ------------------------------------------------------------
    inline double l1_distance(const std::vector<double> &a, const std::vector<double> &b) {
        double s = 0;
        const size_t n = std::min(a.size(), b.size());
        for (size_t i = 0; i < n; ++i) s += std::abs(a[i] - b[i]);
        return s;
    }

    inline double l2_distance(const std::vector<double> &a, const std::vector<double> &b) {
        double s = 0;
        const size_t n = std::min(a.size(), b.size());
        for (size_t i = 0; i < n; ++i) {
            double d = a[i] - b[i];
            s += d * d;
        }
        return std::sqrt(s);
    }

    inline double cosine_similarity(const std::vector<double> &a, const std::vector<double> &b) {
        double dot = 0, na = 0, nb = 0;
        const size_t n = std::min(a.size(), b.size());

        for (size_t i = 0; i < n; ++i) {
            dot += a[i] * b[i];
            na += a[i] * a[i];
            nb += b[i] * b[i];
        }

        if (na == 0 || nb == 0) return 0;
        return dot / (std::sqrt(na) * std::sqrt(nb));
    }

    // ------------------------------------------------------------
    // 从文件对比（JSON -> vector）
    // ------------------------------------------------------------
    inline void compare(const std::string &f1, const std::string &f2) {
        const auto a = load<double>(f1);
        const auto b = load<double>(f2);

        const auto l1 = l1_distance(a, b);
        const auto l2 = l2_distance(a, b);
        const auto cosine = cosine_similarity(a, b);

        std::cout << "L1: " << l1 << "; L2: " << l2 << "; cosine: " << cosine << std::endl;
    }
}
