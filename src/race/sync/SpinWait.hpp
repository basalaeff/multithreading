#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

class SpinWait {
  int spinCounter_ = 0;

 public:
  void spin_once();
  void reset();
};

inline void SpinWait::spin_once() {
  if (spinCounter_ < 10) {
    ++spinCounter_;
  } else if (spinCounter_ < 20) {
    std::this_thread::yield();
    ++spinCounter_;
  } else {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    spinCounter_ = 0;
  }
}

inline void SpinWait::reset() { spinCounter_ = 0; }
