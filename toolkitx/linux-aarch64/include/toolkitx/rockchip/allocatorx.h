#pragma once

#include <sstream>

#define DMA_HEAP_UNCACHE_PATH           "/dev/dma_heap/system-uncached"
#define DMA_HEAP_PATH                   "/dev/dma_heap/system"
#define DMA_HEAP_DMA32_UNCACHED_PATH    "/dev/dma_heap/system-uncached-dma32"
#define DMA_HEAP_DMA32_PATH             "/dev/dma_heap/system-dma32"
#define CMA_HEAP_UNCACHED_PATH          "/dev/dma_heap/cma-uncached"
#define RV1106_CMA_HEAP_PATH            "/dev/rk_dma_heap/rk-dma-heap-cma"

namespace rockchip {
    // ######################################## DMA ################################
    class DmaBuffer {
    public:
        DmaBuffer() = default;

        explicit DmaBuffer(size_t size, const char *path = DMA_HEAP_UNCACHE_PATH);

        ~DmaBuffer() { release(); };

        // 禁止拷贝，防止重复释放
        DmaBuffer(const DmaBuffer &) = delete;

        DmaBuffer &operator=(const DmaBuffer &) = delete;

        DmaBuffer(DmaBuffer &&other) noexcept { move_from(other); }

        DmaBuffer &operator=(DmaBuffer &&other) noexcept {
            if (this != &other) {
                release();
                move_from(other);
            }
            return *this;
        }

        bool alloc(size_t size, const char *path = DMA_HEAP_UNCACHE_PATH);

        [[nodiscard]] DmaBuffer clone() const;

        [[nodiscard]] int sync_device_to_cpu() const;

        [[nodiscard]] int sync_cpu_to_device() const;

        [[nodiscard]] int fd() const { return fd_; }

        [[nodiscard]] void *ptr() const { return ptr_; }

        [[nodiscard]] bool owner() const { return owner_; }

        [[nodiscard]] size_t size() const { return size_; }

        [[nodiscard]] bool empty() const { return (fd_ < 0) || (ptr_ == nullptr) || (size_ == 0); }

        void release();

        [[nodiscard]] std::string to_string() const {
            std::ostringstream oss;
            oss << "DmaBuffer {"
                << " fd=" << fd_
                << ", size=" << size_
                << ", ptr=" << ptr_
                << ", owner=" << std::boolalpha << owner_
                << " }";
            return oss.str();
        }

    private:
        void move_from(DmaBuffer &other);

    private:
        int fd_{-1};
        size_t size_{0};
        bool owner_ = true;
        void *ptr_ = nullptr;
    };

    // ######################################## DRM ################################
    class DrmBuffer {
    public:
        DrmBuffer() = default;

        DrmBuffer(int width, int height, int bytes_per_pixel);

        ~DrmBuffer() { release(); }

        // 禁止拷贝，防止重复释放
        DrmBuffer(const DrmBuffer &) = delete;

        DrmBuffer &operator=(const DrmBuffer &) = delete;

        // Enable move
        DrmBuffer(DrmBuffer &&other) noexcept { move_from(other); }

        DrmBuffer &operator=(DrmBuffer &&other) noexcept {
            if (this != &other) {
                release();
                move_from(other);
            }
            return *this;
        }

        bool alloc(int width, int height, int bytes_per_pixel);

        [[nodiscard]] DrmBuffer clone() const;

        [[nodiscard]] int fd() const { return fd_; }

        [[nodiscard]] void *ptr() const { return ptr_; }

        [[nodiscard]] int width() const { return width_; }

        [[nodiscard]] int height() const { return height_; }

        /// 位深
        [[nodiscard]] int bytes_per_pixel() const { return bytes_per_pixel_; }

        [[nodiscard]] int handle() const { return handle_; }

        /// 每行字节数
        [[nodiscard]] int stride() const { return stride_; }

        /// 总字节
        [[nodiscard]] size_t size() const { return size_; }

        [[nodiscard]] bool empty() const { return (fd_ < 0) || (ptr_ == nullptr) || (size_ == 0); }

        [[nodiscard]] bool owner() const { return owner_; }

        void release();

        [[nodiscard]] std::string to_string() const {
            std::ostringstream oss;
            oss << "DrmBuffer {"
                << " fd=" << fd_
                << ", handle=" << handle_
                << ", width=" << width_
                << ", height=" << height_
                << ", bytes_per_pixel=" << bytes_per_pixel_
                << ", stride=" << stride_
                << ", size=" << size_
                << ", ptr=" << ptr_
                << ", owner=" << std::boolalpha << owner_
                << " }";
            return oss.str();
        }

    private:
        void move_from(DrmBuffer &other);

    private:
        int fd_{-1};
        int width_{0};
        int height_{0};
        int bytes_per_pixel_{0};      ///< bytes per pixel
        int handle_{-1};
        int stride_{0};   ///< stride in bytes
        size_t size_{0};  ///< total bytes
        bool owner_{true};
        void *ptr_{nullptr};
    };
}
