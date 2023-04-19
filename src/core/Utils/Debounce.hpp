#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include "SingletonBase.h"

namespace SPW {

    class MainThreadExecutor : public SingletonBase<MainThreadExecutor> {
    public:
        void push_task(const std::function<void()>& task) {
            std::lock_guard<std::mutex> lock(mutex_);
            task_queue_.push(task);
        }

        void process_tasks() {
            std::lock_guard<std::mutex> lock(mutex_);
            while (!task_queue_.empty()) {
                task_queue_.front()();
                task_queue_.pop();
            }
        }

    private:
        std::queue<std::function<void()>> task_queue_;
        std::mutex mutex_;
    };

    class Debouncer {
    public:
        Debouncer(MainThreadExecutor& executor, std::function<void()> callback, int delay_ms)
            : executor_(executor), callback_(callback), delay_ms_(delay_ms), ready_to_call_(std::chrono::steady_clock::now()), exit_flag_(false) {
            worker_thread_ = std::thread(&Debouncer::worker, this);
        }

        ~Debouncer() {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                exit_flag_ = true;
            }
            cv_.notify_one();
            worker_thread_.join();
        }

        void operator()() {
            std::unique_lock<std::mutex> lock(mutex_);
            ready_to_call_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms_);
            incoming_call_ = true;
            cv_.notify_one();
        }

    private:
        void worker() {
            while (true) {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(delay_ms_), [this]() {
                    return exit_flag_ || std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ready_to_call_).count() >= 0;
                });

                if (exit_flag_) {
                    break;
                }

                if (incoming_call_ && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ready_to_call_).count() >= 0) {
                    executor_.push_task(callback_);
                    incoming_call_ = false;
                    ready_to_call_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms_);
                }
            }
        }

        MainThreadExecutor& executor_;
        std::function<void()> callback_;
        int delay_ms_;
        bool incoming_call_;
        std::chrono::steady_clock::time_point ready_to_call_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::thread worker_thread_;
        bool exit_flag_;
    };
}