#pragma once

#include <string>

namespace toolkitx::crypto {
    class XOR {
    public:
        static std::string encrypt(const std::string &key, const std::string &data);

        static std::string decrypt(const std::string &key, const std::string &data);

        static std::string process(const std::string &key, const std::string &data);
    };

    /**
     * @brief 将二进制字符串编码为十六进制字符串
     *
     * 每个字节转换为两个十六进制字符（范围 "00" 到 "ff"），确保结果仅包含可见 ASCII 字符，
     * 可安全用于文件名、日志记录、网络传输等场景。
     *
     * @param data 原始的二进制字符串（可能包含不可打印字符）
     * @return 对应的十六进制字符串（大小为原始长度的2倍）
     */
    std::string to_hex(const std::string &data);

    /**
     * @brief 将十六进制字符串还原为原始二进制字符串
     *
     * 假设输入为 toHex 函数的结果，每两个字符表示一个字节，还原为原始内容。
     *
     * @param hex 十六进制编码字符串（长度应为偶数）
     * @return 原始的二进制字符串
     */
    std::string from_hex(const std::string &hex);

    void test_xor();
}
