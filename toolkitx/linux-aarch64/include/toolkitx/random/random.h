#pragma once

#include <string>

namespace toolkitx::random {
    void random_sleep(int min = 5, int max = 10);

    bool random_bool(double true_probability = 0.5);

    std::string generate_uid(size_t length = 32);

    // 在字符串中随机插入随机字符，同时删除相同数量的原字符，保持总长度不变。
    // - min_inserts..max_inserts：随机插入次数范围（含端点）
    // - use_upper/lower/digits/punct：选择插入字符集
    // - seed：0 表示用随机设备混合时间做不可复现种子；非 0 表示固定种子（可复现）
    std::string obfuscate_token(
            const std::string &input,
            size_t min_inserts = 1, size_t max_inserts = 5,
            bool use_upper = true, bool use_lower = true,
            bool use_digits = false, bool use_punct = false, uint64_t seed = 0
    );
}
