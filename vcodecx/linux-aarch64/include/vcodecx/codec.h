#pragma once

#include <future>

#include "vcodecx/types.h"

namespace vcodecx {
    class Decoder {
    public:
        using Callback = std::function<void(const std::shared_ptr<FrameX> &)>;

        void release() {
            bool expected = false;
            if (!released_.compare_exchange_strong(expected, true)) {
                return;
            }
            on_release();
        }

        virtual ~Decoder() = default;

        virtual std::string id() = 0;

        virtual bool open() = 0;

        virtual bool read(std::shared_ptr<FrameX> &framex, int timeout_ms) = 0;

        virtual int subscribe(Callback cb) = 0;

        virtual void unsubscribe(int id) = 0;

        virtual CodecState state() = 0;

        [[nodiscard]] virtual StreamInfo stream_info() const = 0;

        [[nodiscard]] virtual DecodeConfig decode_config() const = 0;

        [[nodiscard]] bool is_released() const noexcept {
            return released_.load(std::memory_order_acquire);
        }

    protected:
        /// 子类实现实际释放逻辑，必须保证 noexcept
        virtual void on_release() noexcept = 0;

    private:
        std::atomic<bool> released_{false};
    };

    class Encoder {
    public:
        using Callback = std::function<void(const std::shared_ptr<EncodedX> &)>;

        void release() {
            bool expected = false;
            if (!released_.compare_exchange_strong(expected, true)) {
                return;
            }
            on_release();
        }

        virtual ~Encoder() = default;

        virtual std::string id() = 0;

        virtual bool open() = 0;

        virtual bool write(const std::shared_ptr<FrameX> &item, int timeout_ms) = 0;

        virtual bool read(std::shared_ptr<EncodedX> &encoded, int timeout_ms) = 0;

        virtual int subscribe(Callback cb) = 0;

        virtual void unsubscribe(int id) = 0;

        virtual CodecState state() = 0;

        [[nodiscard]] virtual EncodeConfig encode_config() const = 0;

        [[nodiscard]] bool is_released() const noexcept {
            return released_.load(std::memory_order_acquire);
        }

    protected:
        /// 子类实现实际释放逻辑，必须保证 noexcept
        virtual void on_release() noexcept = 0;

    private:
        std::atomic<bool> released_{false};
    };
}
