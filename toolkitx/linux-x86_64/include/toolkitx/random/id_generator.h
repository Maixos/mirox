#pragma once

#include <atomic>
#include <cstdint>

namespace toolkitx::random {
    class IdGenerator {
    public:
        /// 返回一个全局唯一递增的 ID
        static uint32_t next() {
            return counter_.fetch_add(1, std::memory_order_relaxed);
        }

    private:
        static std::atomic<uint32_t> counter_;
    };
}
