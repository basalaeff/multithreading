#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class SpinLock {
  std::atomic<bool> isLockedFlag_{false};

 public:
  inline void lock();
  inline void unlock();
};

inline void SpinLock::lock() {
  while (isLockedFlag_.exchange(true, std::memory_order_acquire)) {
  }
}

inline void SpinLock::unlock() {
  isLockedFlag_.store(false, std::memory_order_release);
}
