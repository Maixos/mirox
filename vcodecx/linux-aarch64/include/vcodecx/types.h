#pragma once

#include <memory>
#include <utility>
#include <iostream>

namespace vcodecx {
    enum class WorkerMode {
        Callback, // 订阅回调模式（消息到来时立即推送）
        QueueRead // 队列轮询读取模式（消费者主动从队列取）
    };

    enum class MediaType { File, Camera, RTSP };

    enum class CodecType { H264, H265, HEVC };

    enum class ImageFormat {
        RGB24, // RGB 888
        BGR24, // BGR 888
        RGBA32, // RGBA 8888
        BGRA32, // BGRA 8888
        NV12, // Y plane + interleaved UV
        NV21, // Y plane + interleaved VU
        I420, // YUV420P: Y + U + V
        YV12, // Y + V + U
        YUYV422, // Packed 4:2:2 YUYV
        UYVY422 // Packed 4:2:2 UYVY
    };

    enum class CodecState {
        Init, // 刚创建
        Opening, // 正在初始化资源/上下文
        Running, // 正在处理媒体（编码/解码）
        Stopping, // 正在停止线程、关闭 IO
        Stopped, // 停止完成，但可重新开启
        Error, // 错误状态，可尝试重启，也可直接 release
        Released // 完全释放，不可再用
    };

    struct FrameX {
        std::string stream_id{};
        int width{};
        int height{};
        ImageFormat format{};
        int64_t pts{};
        uint32_t timestamp{};
        int fd{-1};
        void *ptr{nullptr};
        std::shared_ptr<void> holder{};

        FrameX() = default;

        FrameX(const FrameX &) = delete;

        FrameX &operator=(const FrameX &) = delete;

        FrameX(FrameX &&) = default;

        FrameX &operator=(FrameX &&) = default;

        FrameX(
                std::string stream_id, const int w, const int h, const ImageFormat format, const int fd, void *ptr,
                const int64_t pts, const uint32_t ts, std::shared_ptr<void> holder
        ) : stream_id(std::move(stream_id)), width(w), height(h), format(format), fd(fd), ptr(ptr), pts(pts),
            timestamp(ts), holder(std::move(holder)) {
        }
    };

    struct EncodedX {
        std::string stream_id{};
        size_t size{};
        int64_t pts{};
        bool is_audio{};      // 是否是音频包
        bool is_keyframe{};
        uint32_t timestamp{};
        uint8_t *data{nullptr};
        std::shared_ptr<void> holder;

        EncodedX() = default;

        EncodedX(const EncodedX &) = delete;

        EncodedX &operator=(const EncodedX &) = delete;

        EncodedX(EncodedX &&) = default;

        EncodedX &operator=(EncodedX &&) = default;

        EncodedX(
                std::string stream_id, uint8_t *data, const size_t size, bool is_keyframe, const int64_t pts,
                const uint32_t ts, std::shared_ptr<void> holder, bool is_audio = false
        ) : stream_id(std::move(stream_id)), size(size), pts(pts), is_keyframe(is_keyframe), is_audio(is_audio),
            timestamp(ts), data(data), holder(std::move(holder)) {
        }
    };

    struct StreamInfo {
        std::string uri{};
        std::string name{};
        std::string stream_id{};

        StreamInfo() = default;

        explicit StreamInfo(std::string id, std::string uri, std::string name = {})
                : stream_id(std::move(id)), uri(std::move(uri)), name(std::move(name)) {
        }
    };

    struct DecodeConfig {
        int width;
        int height;
        int max_fps;
        int max_queue_size;
        WorkerMode worker_mode;
        ImageFormat output_format;

        DecodeConfig() = default;

        explicit DecodeConfig(
                const int w, const int h, const ImageFormat fmt = ImageFormat::BGR24,
                const WorkerMode mode = WorkerMode::QueueRead, const int fps = 60, const int cap = 5
        ) : width(w), height(h), output_format(fmt), worker_mode(mode), max_fps(fps), max_queue_size(cap) {
        }
    };

    struct EncodeConfig {
        int width;
        int height;
        int max_fps;
        int max_queue_size;
        CodecType codec_type;
        WorkerMode worker_mode;

        EncodeConfig() = default;

        explicit EncodeConfig(
                const int w, const int h, const WorkerMode mode = WorkerMode::QueueRead,
                const int fps = 30, const int queue_size = 5, const CodecType codec_type = CodecType::H265
        ) : width(w), height(h), worker_mode(mode), max_fps(fps), max_queue_size(queue_size), codec_type(codec_type) {
        }
    };

    // ===========================================================
    //                      枚举类型转换
    // ===========================================================
    std::string worker_mode_to_string(WorkerMode mode);

    bool worker_mode_from_string(std::string &str, WorkerMode &mode);

    std::string media_type_to_string(MediaType type);

    bool media_type_from_string(std::string &str, MediaType &type);

    std::string codec_type_to_string(CodecType type);

    bool codec_type_from_string(std::string &str, CodecType &type);

    std::string image_format_to_string(ImageFormat fmt);

    bool image_format_from_string(std::string &str, ImageFormat &fmt);
}
