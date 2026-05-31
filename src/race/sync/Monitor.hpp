#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

struct Monitor {
  std::mutex monitorMutex_;
  std::condition_variable monitorCv_;
  bool isReady_ = true;
  void enter();
  void exit();
};

inline void Monitor::enter() {
  std::unique_lock<std::mutex> lock(monitorMutex_);
  monitorCv_.wait(lock, [this] { return isReady_; });

  isReady_ = false;
}

inline void Monitor::exit() {
  std::lock_guard<std::mutex> lock(monitorMutex_);
  isReady_ = true;
  monitorCv_.notify_one();
}
