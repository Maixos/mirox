#pragma once

#include <memory>
#include <cstdint>
#include <cstring>

namespace rtspx {
    static constexpr int MAX_MEDIA_TRACK = 2;

    enum MediaTrack {
        Video,
        Audio
    };

    enum CodecType {
        //PCMU = 0,
        PCMA = 8,
        H264 = 96,
        AAC = 37,
        H265 = 265,
        NONE
    };

    enum FrameType {
        VIDEO_FRAME_I = 0x01,
        VIDEO_FRAME_P = 0x02,
        VIDEO_FRAME_B = 0x03,
        AUDIO_FRAME = 0x11,
    };

    struct EncodedOwned {
        size_t size{};
        int64_t pts{};
        uint32_t rtp_timestamp{};
        std::shared_ptr<uint8_t> data; // own copy
        FrameType frame_type{VIDEO_FRAME_P};

        EncodedOwned() = default;

        EncodedOwned(
                const uint8_t *src, size_t size, int64_t pts = 0, uint32_t rtp_timestamp = 0,
                FrameType frame_type = VIDEO_FRAME_P
        ) : size(size), pts(pts), rtp_timestamp(rtp_timestamp), frame_type(frame_type),
            data(new uint8_t[size], std::default_delete<uint8_t[]>()) {
            std::memcpy(data.get(), src, size);
        }
    };

    struct EncodedShared {
        uint8_t *data{nullptr}; // zero-copy
        size_t size{};
        int64_t pts{}; // media time (FFmpeg pts)
        uint32_t rtp_timestamp{}; // user/system/business time
        FrameType frame_type{};
        std::shared_ptr<void> holder{}; // zero-copy memory keeper

        EncodedShared() = default;

        EncodedShared(
                uint8_t *data, size_t size, int64_t pts, uint32_t rtp_timestamp, std::shared_ptr<void> holder,
                FrameType frame_type = VIDEO_FRAME_P
        ) : data(data), size(size), holder(std::move(holder)), pts(pts), rtp_timestamp(rtp_timestamp),
            frame_type(frame_type) {
        }
    };
}
