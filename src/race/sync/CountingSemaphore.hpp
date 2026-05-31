#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class CountingSemaphore {
  std::mutex semMutex_;
  std::condition_variable semCv_;
  unsigned int permits_;

 public:
  explicit CountingSemaphore(unsigned int initial = 1);
  void acquire();
  void release();
};

inline CountingSemaphore::CountingSemaphore(unsigned int initial)
    : permits_(initial) {}

inline void CountingSemaphore::acquire() {
  std::unique_lock<std::mutex> lock(semMutex_);
  semCv_.wait(lock, [this] { return permits_ > 0; });
  --permits_;
}

inline void CountingSemaphore::release() {
  std::lock_guard<std::mutex> lock(semMutex_);
  ++permits_;
  semCv_.notify_one();
}
