#pragma once

#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <algorithm>
#include <functional>

namespace toolkitx::event {
    template<typename T>
    class PubSubBus {
    public:
        using Ptr = std::shared_ptr<T>;
        using Callback = std::function<void(const Ptr &)>;

        int num_subscribers() { return subscribers_.size(); }

        int subscribe(Callback cb, const std::string &name = {}) {
            std::lock_guard<std::mutex> lock(mtx_);
            int id = next_id_++;
            subscribers_.push_back(std::make_unique<Subscriber>(id, std::move(cb), name));
            return id;
        }

        void unsubscribe(int id) {
            std::lock_guard<std::mutex> lock(mtx_);
            for (auto &sub: subscribers_) {
                if (sub->id == id) {
                    sub->active = false;
                    break;
                }
            }
            remove_inactive();
        }

        void publish(const Ptr &data) {
            std::lock_guard<std::mutex> lock(mtx_);
            for (auto &sub: subscribers_) {
                if (sub->active.load())
                    sub->cb(data);
            }
        }

    private:
        struct Subscriber {
            int id{};
            Callback cb;
            std::string name;
            std::atomic<bool> active{true};

            Subscriber(int id_, Callback cb_, std::string name_)
                    : id(id_), cb(std::move(cb_)), name(std::move(name_)), active(true) {}
        };

        void remove_inactive() {
            subscribers_.erase(
                    std::remove_if(subscribers_.begin(), subscribers_.end(),
                                   [](const auto &s) { return !s->active.load(); }),
                    subscribers_.end()
            );
        }

    private:
        std::mutex mtx_;
        std::atomic<int> next_id_{0};
        std::vector<std::unique_ptr<Subscriber>> subscribers_;
    };
}

/*
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <iostream>

struct RawFrame {
    int id;
    // 图像像素数据，例如 YUV/DRM fd 等
};

struct EncodedPacket {
    int id;
    std::vector<uint8_t> data; // 编码后数据
};

// 编码器类
class Encoder {
public:
    Encoder() : running_(false) {}

    void start() {
        running_ = true;
        worker_ = std::thread(&Encoder::encodeLoop, this);
    }

    void stop() {
        running_ = false;
        cv_.notify_all();
        if (worker_.joinable())
            worker_.join();
    }

    // 外部推送原始帧
    void pushFrame(const std::shared_ptr<RawFrame>& frame) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            inputQueue_.push(frame);
        }
        cv_.notify_one();
    }

    // 获取输出 Bus（下游订阅用）
    PubSubBus<EncodedPacket>& bus() { return bus_; }

private:
    void encodeLoop() {
        while (running_) {
            std::shared_ptr<RawFrame> frame;
            {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [&] { return !inputQueue_.empty() || !running_; });
                if (!running_) break;
                frame = inputQueue_.front();
                inputQueue_.pop();
            }

            // 模拟编码过程
            auto pkt = std::make_shared<EncodedPacket>();
            pkt->id = frame->id;
            pkt->data = fakeEncode(frame);  // 伪函数

            // 发布给所有订阅者
            bus_.publish(pkt);
        }
    }

    std::vector<uint8_t> fakeEncode(const std::shared_ptr<RawFrame>& frame) {
        // 实际会调用 FFmpeg/NVENC/RKMPP 等，这里仅模拟
        std::vector<uint8_t> encoded(100, (uint8_t)frame->id);
        return encoded;
    }

private:
    std::queue<std::shared_ptr<RawFrame>> inputQueue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic<bool> running_;
    std::thread worker_;

    PubSubBus<EncodedPacket> bus_;  // 输出总线
};

class Recorder {
public:
    Recorder(PubSubBus<EncodedPacket>& bus) {
        sub_id_ = bus.subscribe([this](const std::shared_ptr<EncodedPacket>& pkt){
            this->onPacket(pkt);
        });
    }

    void onPacket(const std::shared_ptr<EncodedPacket>& pkt) {
        std::cout << "[Recorder] writing packet " << pkt->id << std::endl;
        // 实际写入文件
    }

private:
    int sub_id_;
};

class Streamer {
public:
    Streamer(PubSubBus<EncodedPacket>& bus) {
        sub_id_ = bus.subscribe([this](const std::shared_ptr<EncodedPacket>& pkt){
            this->onPacket(pkt);
        });
    }

    void onPacket(const std::shared_ptr<EncodedPacket>& pkt) {
        std::cout << "[Streamer] sending packet " << pkt->id << std::endl;
        // 实际通过 RTSP/RTMP 推流
    }

private:
    int sub_id_;
};

int main() {
    Encoder encoder;

    // 下游订阅
    Recorder recorder(encoder.bus());
    Streamer streamer(encoder.bus());

    encoder.start();

    // 模拟输入帧
    for (int i = 0; i < 5; i++) {
        auto frame = std::make_shared<RawFrame>();
        frame->id = i;
        encoder.pushFrame(frame);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    encoder.stop();

    return 0;
}
*/
