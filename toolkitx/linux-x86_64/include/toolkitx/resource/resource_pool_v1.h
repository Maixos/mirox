/**
 * @file    resource_pool_v1.h
 * @brief   通用资源池模板类，用于管理任意类型的可复用资源
 *
 * ResourcePoolV1 是一个通用的对象池/资源管理工具，可以管理任意类型的资源（例如
 * GPU buffer、内存块、连接对象等）。它通过模板化设计，实现资源的统一管理和复用，
 * 支持多线程安全访问。
 *
 * 特性：
 *   - 池中的资源类型由模板参数 T 决定，可用于任意类型
 *   - 资源所有权由 ResourcePoolV1 管理
 *   - acquire() 返回 std::shared_ptr<T>，带自定义 deleter
 *   - 支持阻塞、超时、非阻塞获取
 *   - 可选惰性初始化（第一次 acquire 时用户自行创建资源）
 *   - 多线程安全，内部使用 mutex + condition_variable 管理可用资源
 *
 * 使用示例（支持第一次惰性创建）：
 *   ResourcePoolV1<MyResource> pool(8); // 创建 8 个池项
 *
 *   auto res = pool.acquire();         // 获取资源
 *   if (!res) {
 *       // 获取失败（超时或池空）
 *   }
 *   if (!res->data()) {                // 第一次使用，资源未创建
 *       auto new_resource = std::make_unique<MyResource>(...);
 *       res->assign(std::move(new_resource)); // 将资源赋给池项
 *   }
 *
 *   // 使用资源
 *   res->data()->doSomething();
 *
 *   // 自动归还：当 shared_ptr 超出作用域或调用 reset() 时，资源被标记可用
 *
 * 注意事项：
 *   1. 如果使用覆盖队列或短生命周期容器，应及时 reset()，确保资源归还池
 *   2. acquire() 返回的 shared_ptr 生命周期管理资源，用户无需手动 release
 *   3. 可以延迟分配资源（第一次 acquire 时创建），提高启动性能
 *   4. 线程安全，可在多线程环境下安全使用
 *
 * @author MincoX
 * @date
 * @version 1.0
 */
#pragma once

#include <vector>
#include <atomic>
#include <condition_variable>

namespace toolkitx::respool {
    template<typename T>
    class ResourcePoolV1 {
    public:
        explicit ResourcePoolV1(size_t pool_size) : items_(std::max<size_t>(1, pool_size)) {
            for (auto &item: items_) {
                item.data = std::make_unique<T>();
                item.in_use.store(false);
            }
        }

        ~ResourcePoolV1() noexcept {
            destroy();
        }

        ResourcePoolV1(const ResourcePoolV1 &) = delete;

        ResourcePoolV1 &operator=(const ResourcePoolV1 &) = delete;

        /// 获取资源，自动回收
        std::shared_ptr<T> acquire(int timeout_ms = -1) {
            std::unique_lock<std::mutex> lock(mtx_);
            Item *item = try_acquire(timeout_ms, lock);
            if (!item) return nullptr;

            // 捕获 this 和 item，在资源析构时自动归还资源
            return std::shared_ptr<T>(
                    item->data.get(), [this, item](T *) {
                        std::lock_guard<std::mutex> g(mtx_);
                        item->in_use.store(false);
                        cv_.notify_one();
                    }
            );
        }

        size_t size() const noexcept {
            return items_.size();
        }

        size_t acquired() const {
            std::lock_guard<std::mutex> g(mtx_);
            size_t count = 0;
            for (const auto &item: items_) {
                if (item.in_use.load()) ++count;
            }
            return count;
        }

        size_t available() const {
            return size() - acquired();
        }

    private:
        struct Item {
            std::unique_ptr<T> data{};
            std::atomic<bool> in_use{false};
        };

        Item *try_acquire(int timeout_ms, std::unique_lock<std::mutex> &lock) {
            Item *result = nullptr;
            auto pred = [&]() {
                result = find_free_item();
                return result != nullptr;
            };

            if (timeout_ms < 0) {
                cv_.wait(lock, pred);
            } else if (timeout_ms == 0) {
                if (!pred()) return nullptr;
            } else {
                if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms), pred)) {
                    return nullptr;
                }
            }
            return result;
        }

        Item *find_free_item() {
            for (size_t i = 0; i < items_.size(); ++i) {
                size_t idx = (last_index_ + i) % items_.size();
                if (!items_[idx].in_use.exchange(true)) {
                    last_index_ = (idx + 1) % items_.size();
                    return &items_[idx];
                }
            }
            return nullptr;
        }

        void destroy() noexcept {
            std::lock_guard<std::mutex> lk(mtx_);
            for (auto &item: items_) {
                item.data.reset();
                item.in_use.store(false);
            }
            cv_.notify_all();
        }

    private:
        size_t last_index_ = 0;
        std::vector<Item> items_;
        mutable std::mutex mtx_;
        std::condition_variable cv_;
    };
}
