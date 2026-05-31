#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class SpinWaitMutex {
  std::atomic<bool> isLockedFlag_{false};
  SpinWait spinWaiter_;

 public:
  void lock();
  void unlock();
};

inline void SpinWaitMutex::lock() {
  spinWaiter_.reset();
  bool expectedUnlocked = false;

  while (!isLockedFlag_.compare_exchange_weak(expectedUnlocked, true,
                                              std::memory_order_acquire)) {
    expectedUnlocked = false;
    spinWaiter_.spin_once();
  }
}

inline void SpinWaitMutex::unlock() {
  isLockedFlag_.store(false, std::memory_order_release);
}
