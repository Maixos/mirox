#pragma once

#include <string>

namespace logx {

    class Logger {
    public:
        /**
         * 初始化日志系统（唯一入口）
         *
         * @param name             日志器名称
         * @param level            日志级别字符串：trace/debug/info/warn/error/critical/off
         * @param enable_file_sink 是否启用文件输出
         * @param async_mode       是否启用异步
         * @param log_root         日志文件根目录（当启用文件输出时生效）
         *
         * 说明：
         *   - 多次调用时，当前实现会复用已有 logger，只更新日志等级。
         */
        static void init(
                const std::string &name,
                const std::string &level = "info",
                bool enable_file_sink = false,
                bool async_mode = true,
                const std::string &log_root = "logs"
        );

        static void log(int level, const std::string &msg);
    };

} // namespace logx

//------------------------------
// 日志宏
//------------------------------
#define LOGX_LOG(level, MODULE, MSG, ...) \
    do { \
        char buf[1024]; \
        std::snprintf(buf, sizeof(buf), "[%s] " MSG, MODULE, ##__VA_ARGS__); \
        ::logx::Logger::log(level, std::string(buf)); \
    } while (0)

#define LOG_TRACE(MODULE, MSG, ...) LOGX_LOG(0, MODULE, MSG, ##__VA_ARGS__)
#define LOG_DEBUG(MODULE, MSG, ...) LOGX_LOG(1, MODULE, MSG, ##__VA_ARGS__)
#define LOG_INFO(MODULE, MSG, ...) LOGX_LOG(2, MODULE, MSG, ##__VA_ARGS__)
#define LOG_WARN(MODULE, MSG, ...) LOGX_LOG(3, MODULE, MSG, ##__VA_ARGS__)
#define LOG_ERROR(MODULE, MSG, ...) LOGX_LOG(4, MODULE, MSG, ##__VA_ARGS__)
#define LOG_CRIT(MODULE, MSG, ...) LOGX_LOG(5, MODULE, MSG, ##__VA_ARGS__)
