#pragma once

#include <queue>
#include <atomic>
#include <algorithm>
#include <condition_variable>

namespace toolkitx::respool
{
    template <class T>
    class ResourcePoolV2
    {
    public:
        class Slot : public std::enable_shared_from_this<Slot>
        {
        public:
            std::shared_ptr<T>& data() noexcept { return data_; }

            void recycle() noexcept
            {
                if (!recycled_.exchange(true))
                {
                    if (pool_) pool_->recycle(this);
                }
            }

        private:
            friend class ResourcePoolV2;

            explicit Slot(ResourcePoolV2* pool) : pool_(pool)
            {
            }

        private:
            ResourcePoolV2* pool_{};
            std::shared_ptr<T> data_{};

            std::atomic<bool> recycled_{true};
        };

        using SlotPtr = std::shared_ptr<Slot>;

        explicit ResourcePoolV2(size_t size) : running_(true), capacity_(size), num_available_(size)
        {
            slots_.reserve(size);
            for (size_t i = 0; i < size; ++i)
            {
                slots_.emplace_back(std::shared_ptr<Slot>(new Slot(this)));
                available_indices_.push(i);
            }
        }

        ~ResourcePoolV2()
        {
            stop();
        }

        SlotPtr acquire(const int timeout_ms = 10)
        {
            std::unique_lock<std::mutex> lk(mutex_);

            if (!running_) return nullptr;

            bool ok = false;

            if (timeout_ms < 0)
            {
                cv_.wait(lk, [&]() { return !available_indices_.empty() || !running_; });
                ok = !available_indices_.empty() && running_;
            }
            else
            {
                ok = cv_.wait_for(lk, std::chrono::milliseconds(timeout_ms), [&]()
                {
                    return !available_indices_.empty() || !running_;
                });
            }

            if (!ok || !running_ || available_indices_.empty()) return nullptr;

            size_t idx = available_indices_.front();
            available_indices_.pop();

            auto item = slots_[idx];
            item->recycled_.store(false, std::memory_order_release);
            --num_available_;

            return item;
        }

        size_t num_available() const noexcept { return num_available_.load(); }
        size_t capacity() const noexcept { return capacity_; }

    private:
        void recycle(Slot* item) noexcept
        {
            std::unique_lock<std::mutex> lk(mutex_);

            auto it = std::find_if(
                slots_.begin(), slots_.end(),
                [&](const SlotPtr& p) { return p.get() == item; }
            );

            if (it != slots_.end())
            {
                const size_t idx = std::distance(slots_.begin(), it);
                available_indices_.push(idx);
                ++num_available_;
                cv_.notify_one();
            }
        }

        void stop() noexcept
        {
            std::unique_lock<std::mutex> lk(mutex_);
            running_ = false;
            cv_.notify_all();
        }

    private:
        mutable std::mutex mutex_;
        std::atomic<bool> running_;
        std::condition_variable cv_;

        std::vector<SlotPtr> slots_;

        const size_t capacity_;
        std::atomic<size_t> num_available_;
        std::queue<size_t> available_indices_;
    };
}
