#pragma once

#include <mutex>
#include <functional>
#include <unordered_set>

#include "rtspx/types.h"

namespace rtspx {
    class RtpConnection;

    class MediaSession {
    public:
        using Ptr = std::shared_ptr<MediaSession>;
        using NotifyConnectedCallback = std::function<void(uint32_t session_id, std::string ip, uint16_t port)>;
        using NotifyDisconnectedCallback = std::function<void (uint32_t session_id, std::string ip, uint16_t port)>;

        virtual ~MediaSession() = default;

        virtual bool add_source(MediaTrack channel_id, CodecType type) = 0;

        virtual bool remove_source(MediaTrack channel_id) = 0;

        virtual bool start_multicast() = 0;

        virtual void add_connected_notify_callback(const NotifyConnectedCallback &callback) = 0;

        virtual void add_disconnected_notify_callback(const NotifyDisconnectedCallback &callback) = 0;

        [[nodiscard]] virtual std::string get_url_suffix() const = 0;

        virtual void set_url_suffix(std::string &suffix) = 0;

        virtual std::string get_sdp_message(std::string ip, std::string session_name) = 0;

        virtual bool push_data(MediaTrack channel_id, const EncodedOwned &frame) = 0;

        virtual bool push_data(MediaTrack channel_id, const EncodedShared &frame) = 0;

        virtual bool add_client(int rtspfd, std::shared_ptr<RtpConnection> rtp_conn) = 0;

        virtual void remove_client(int rtspfd) = 0;

        virtual uint32_t get_session_id() = 0;

        [[nodiscard]] virtual uint32_t get_num_client() const = 0;

        [[nodiscard]] virtual bool is_multicast() const = 0;

        [[nodiscard]] virtual std::string get_multicast_ip() const = 0;

        [[nodiscard]] virtual uint16_t get_multicast_port(MediaTrack channel_id) const = 0;

        [[nodiscard]] virtual bool has_channel(MediaTrack channel_id) const = 0;

        [[nodiscard]] virtual uint32_t get_clock_rate(MediaTrack channel_id) const = 0;

        [[nodiscard]] virtual uint8_t get_payload_type(MediaTrack channel_id) const = 0;

        static std::shared_ptr<MediaSession> create(const std::string &url_suffix = "live");
    };

    class MulticastAddr {
    public:
        static MulticastAddr &instance();

        MulticastAddr(const MulticastAddr &) = delete;

        MulticastAddr &operator=(const MulticastAddr &) = delete;

        std::string get_address();

        void release(const std::string &addr);

    private:
        MulticastAddr() = default;

        ~MulticastAddr() = default;

    private:
        std::mutex mutex_;
        std::unordered_set<std::string> addresses_;
    };
}
