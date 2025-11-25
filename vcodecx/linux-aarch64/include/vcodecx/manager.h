#pragma once

#include <unordered_map>

#include "vcodecx/codec.h"

namespace vcodecx {
    class Manager {
    public:
        virtual ~Manager() = default;

        /// ---------------- 解码器 ----------------
        virtual std::shared_ptr<Decoder> create_decoder(const StreamInfo &, const DecodeConfig &) = 0;

        virtual CodecState get_decoder_state(const std::string &stream_id) = 0;

        virtual std::shared_ptr<Decoder> get_decoder(const std::string &stream_id) = 0;

        virtual std::unordered_map<std::string, std::shared_ptr<Decoder> > get_all_decoders() = 0;

        virtual int subscribe_decoder(const std::string &stream_id, Decoder::Callback cb) = 0;

        virtual void unsubscribe_decoder(const std::string &stream_id, int subscribe_id) = 0;

        virtual bool start_decoder(const std::string &stream_id) = 0;

        virtual bool release_decoder(const std::string &stream_id) = 0;

        /// ---------------- 编码器 ----------------
        virtual std::shared_ptr<Encoder> create_encoder(const EncodeConfig &config) = 0;

        virtual CodecState get_encoder_state(const std::string &id) = 0;

        virtual std::shared_ptr<Encoder> get_encoder(const std::string &id) = 0;

        virtual std::unordered_map<std::string, std::shared_ptr<Encoder> > get_all_encoders() = 0;

        virtual int subscribe_encoder(const std::string &id, Encoder::Callback cb) = 0;

        virtual void unsubscribe_encoder(const std::string &id, int subscribe_id) = 0;

        virtual bool start_encoder(const std::string &id) = 0;

        virtual bool release_encoder(const std::string &key) = 0;

        /// ---------------- 初始化入口 ----------------
        static std::shared_ptr<Manager> create();
    };
}
