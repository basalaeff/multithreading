#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>

class Barrier {
  std::atomic<int> arrivedCount_{0};
  std::atomic<int> generation_{0};
  const int totalThreads_;

  // Добавил синхронизацию как члены класса:
  mutable std::mutex barrierMutex_;
  mutable std::condition_variable barrierCv_;

 public:
  explicit Barrier(int count) : totalThreads_(count) {}
  void arrive_and_wait();
};

inline void Barrier::arrive_and_wait() {
  std::unique_lock<std::mutex> lock(barrierMutex_);
  int my_gen = generation_.load(std::memory_order_relaxed);
  int new_count = ++arrivedCount_;

  if (new_count == totalThreads_) {
    arrivedCount_.store(0, std::memory_order_relaxed);
    generation_.fetch_add(1, std::memory_order_relaxed);
    barrierCv_.notify_all();
  } else {
    barrierCv_.wait(lock, [this, my_gen]() {
      return generation_.load(std::memory_order_relaxed) != my_gen;
    });
  }
}