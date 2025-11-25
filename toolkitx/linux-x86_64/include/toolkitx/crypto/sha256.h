#pragma once

#include <vector>
#include <sstream>

namespace toolkitx::crypto {
    class SHA256 {
    public:
        SHA256();

        static std::string hash(const std::string &input);

    private:
        static const uint32_t k[64];
        uint32_t h[8]{};
        uint64_t bit_length;
        std::vector<uint8_t> buffer;

        void process_block();

        uint32_t rotate_right(uint32_t value, uint32_t bits);

        void update(const std::string &input);

        std::string finalize();
    };

    void test_sha256();
}
