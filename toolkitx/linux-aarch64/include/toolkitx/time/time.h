#pragma once

#include <string>
#include <chrono>

namespace toolkitx::time {
    std::chrono::system_clock::time_point parse_date_str(const std::string &dateStr);

    std::string get_time_string();

    int64_t get_timestamp();

    std::string timestamp_to_string(int64_t timestamp);

    std::string time_point_to_string(const std::chrono::system_clock::time_point &tp);

    std::chrono::system_clock::time_point timestampstr_to_point(const std::string &timestamp_str);

    time_t get_specified_time(int year, int month, int day, int hour, int minute, int second);
}
