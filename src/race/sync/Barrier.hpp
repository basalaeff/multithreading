#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class Barrier {
  std::atomic<int> arrivedCount_{0};
  std::atomic<int> generation_{0};
  const int totalThreads_;

 public:
  explicit Barrier(int count) : totalThreads_(count) {}
  void arrive_and_wait();
};

inline void Barrier::arrive_and_wait() {
  static std::mutex global_mutex_;
  static std::condition_variable global_cv_;
  static int waiting = 0;
  static int generation_ = 0;

  std::unique_lock<std::mutex> lock(global_mutex_);

  int my_gen = generation_;
  waiting++;

  if (waiting == totalThreads_) {
    waiting = 0;
    generation_++;
    global_cv_.notify_all();
  } else {
    global_cv_.wait(lock, [&]() { return generation_ != my_gen; });
  }
}
