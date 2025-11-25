#pragma once

#include <memory>
#include <string>

#include "rtspx/sessionx.h"

namespace rtspx {
    class RtspServer {
    public:
        virtual ~RtspServer() = default;

        virtual bool start(std::string ip, uint16_t port) = 0;

        virtual void stop() = 0;

        virtual std::shared_ptr<MediaSession> add_session(std::string suffix) = 0;

        virtual void remove_session(uint32_t sessionId) = 0;

        virtual bool push_frame(uint32_t sessionId, MediaTrack channelId, EncodedOwned frame) = 0;

        static std::shared_ptr<RtspServer> create(
            bool auth = false, const std::string &username = "mincox", const std::string &password = "guess"
        );
    };
}
