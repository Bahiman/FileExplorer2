#include <functional>
#include <future>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

class CancellableThreadPool {
public:
    void init(int num);
    void terminate(); // stop and process all delegated tasks
    void cancel();    // stop and drop all tasks remained in queue

    bool inited() const;
    bool is_running() const;
    int size() const;

    template <class F, class... Args>
    auto async(F&& f, Args&&... args) const->std::future<decltype(f(args...))>;

private:
    std::vector<std::jthread> workers_;
    std::queue<std::function<void(std::stop_token)>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
};

void CancellableThreadPool::init(int num) {
    for (int i = 0; i < num; ++i) {
        workers_.emplace_back([this](std::stop_token stop_token) {
            while (true) {
                std::function<void(std::stop_token)> task;

                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this, stop_token] { return stop_ || !tasks_.empty() || stop_token.stop_requested(); });

                    if ((stop_ && tasks_.empty()) || stop_token.stop_requested()) {
                        return;
                    }

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                task(stop_token);
            }
            });
    }
}

void CancellableThreadPool::terminate() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }

    cv_.notify_all();

    for (std::jthread& worker : workers_) {
        worker.join();
    }
}

void CancellableThreadPool::cancel() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }

    cv_.notify_all();

    for (std::jthread& worker : workers_) {
        worker.request_stop();
        worker.join();
    }
}

template <class F, class... Args>
auto CancellableThreadPool::async(F&& f, Args&&... args) const -> std::future<decltype(f(args...))> {
    auto task = std::make_shared<std::packaged_task<decltype(f(args...))(std::stop_token)>>(std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Args>(args)...));

    {
        std::lock_guard lock(mutex_);
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks_.emplace([task](std::stop_token stop_token) { (*task)(stop_token); });
    }

    cv_.notify_all();

    return task->get_future();
}
