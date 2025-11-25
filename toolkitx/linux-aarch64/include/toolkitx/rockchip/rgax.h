#pragma once

#include <memory>
#include <numeric>

#include "toolkitx/rockchip/rga_types.h"
#include "toolkitx/rockchip/allocatorx.h"

namespace rockchip {
    static inline bool is_aligned(int width, int bytes_per_pixel, int align_bytes) {
        const int stride = width * bytes_per_pixel;
        return stride % align_bytes == 0;
    }

    static inline int align_up(int width, int bytes_per_pixel, int align_bytes) {
        const int pixel_align = align_bytes / std::gcd(align_bytes, bytes_per_pixel);
        return ((width + pixel_align - 1) / pixel_align) * pixel_align;
    }

    static inline int get_bits_per_pixel(int fmt) {
        switch (fmt) {
            case RK_FORMAT_RGBA_8888:
            case RK_FORMAT_BGRA_8888:
            case RK_FORMAT_ARGB_8888:
            case RK_FORMAT_ABGR_8888:
            case RK_FORMAT_RGBX_8888:
            case RK_FORMAT_BGRX_8888:
                return 32;

            case RK_FORMAT_RGB_888:
            case RK_FORMAT_BGR_888:
                return 24;

            case RK_FORMAT_RGB_565:
            case RK_FORMAT_BGR_565:
                return 16;

            case RK_FORMAT_YCrCb_420_SP:
            case RK_FORMAT_YCbCr_420_SP:
            case RK_FORMAT_YCrCb_420_P:
            case RK_FORMAT_YCbCr_420_P:
                return 12;  // 12 bits per pixel → 1.5 bytes

            case RK_FORMAT_YCbCr_400:
            case RK_FORMAT_Y4:
                return 8;

            default:
                return 0;
        }
    }

    /*
     * DMA 是按行读取的，每行 stride 必须对齐 64 字节，否则 RGA 会报 EINVAL
    */
    class RgaX {
    public:
        virtual ~RgaX() = default;

        virtual bool transform(
                int src_fd, int src_w, int src_h, RgaSURF_FORMAT src_fmt,
                int dst_fd, int dst_w, int dst_h, RgaSURF_FORMAT dst_fmt
        ) = 0;

        virtual bool transform(
                void *src_vir, int src_w, int src_h, RgaSURF_FORMAT src_fmt,
                int dst_fd, int dst_w, int dst_h, RgaSURF_FORMAT dst_fmt
        ) = 0;

        virtual bool transform(
                void *src_vir, int src_w, int src_h, RgaSURF_FORMAT src_fmt,
                void *dst_vir, int dst_w, int dst_h, RgaSURF_FORMAT dst_fmt
        ) = 0;

        virtual bool transform(
                int src_fd, int src_w, int src_h, RgaSURF_FORMAT src_fmt,
                void *dst_vir, int dst_w, int dst_h, RgaSURF_FORMAT dst_fmt
        ) = 0;

        virtual bool copy_to_batch(
                int src_fd, int dst_fd, int width, int height, int offset_batch, int batch_size, RgaSURF_FORMAT fmt
        ) = 0;

        virtual bool copy_from_batch(
                int src_fd, int dst_fd, int width, int height, int offset_batch, int batch_size, RgaSURF_FORMAT fmt
        ) = 0;

        virtual DrmBuffer img_to_buffer(void *src_vir, int src_w, int src_h, RgaSURF_FORMAT fmt) = 0;

        virtual bool letterbox(
                void *src_vir, int src_w, int src_h, RgaSURF_FORMAT src_fmt,
                int dst_fd, int dst_w, int dst_h, RgaSURF_FORMAT dst_fmt,
                float scale, int pad_x, int pad_y
        ) = 0;

        virtual DrmBuffer letterbox(
                void* src_vir, int src_w, int src_h, RgaSURF_FORMAT src_fmt,
                int dst_w, int dst_h, RgaSURF_FORMAT dst_fmt,
                float scale, int pad_x, int pad_y, uint8_t color
        ) = 0;

        virtual void destroy() = 0;

        static std::shared_ptr<RgaX> instance(const std::string &lib_path = "/usr/lib/aarch64-linux-gnu/librga.so");
    };
}
