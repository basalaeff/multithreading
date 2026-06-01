```
#include "run.hpp"

int main() {
  std::cout << "Число потоков: " << count_threads
            << ", Итераций на поток: " << count_iterations << "\n\n";

  {
    std::mutex stdMutex;
    run_benchmark("std::mutex", race_with_mutex, stdMutex);
  }

  {
    SpinLock spinLock;
    run_benchmark("SpinLock", race_with_spinlock, spinLock);
  }

  {
    SpinWaitMutex spinWaitMutex;
    run_benchmark("SpinWaitMutex", race_with_spinwait_mutex, spinWaitMutex);
  }

  {
    Monitor monitor;
    run_benchmark("Monitor", race_with_monitor, monitor);
  }

  {
    CountingSemaphore semaphore(1);
    run_benchmark("CountingSemaphore", race_with_counting_semaphore, semaphore);
  }

  {
    Barrier barrier(count_threads);
    run_benchmark_barrier("Barrier", barrier);
  }

  return 0;
}
```

```
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "tasks/race_tasks.hpp"

// количество потоков
extern const int count_threads{4};

template <typename WorkFunc, typename SyncType>
void run_benchmark(const std::string& benchmarkName, WorkFunc work,
                   SyncType& sync) {
  std::cout << " Запуск: " << benchmarkName << " ... " << std::flush;

  std::string sharedTextBuffer;
  sharedTextBuffer.reserve(count_threads * count_iterations);

  auto time_start = std::chrono::high_resolution_clock::now();

  std::vector<std::thread> workerThreads;
  workerThreads.reserve(count_threads);
  for (int threadIndex = 0; threadIndex < count_threads; ++threadIndex) {
    workerThreads.emplace_back(work, ref(sharedTextBuffer), std::ref(sync));
  }

  for (auto& th : workerThreads) {
    th.join();
  }

  auto time_end = std::chrono::high_resolution_clock::now();
  auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(
                       time_end - time_start)
                       .count();

  if (sharedTextBuffer.size() != (size_t)(count_threads * count_iterations)) {
    std::cout << "[ОШИБКА РАЗМЕРА] ";
  }

  std::cout << "Время: " << elapsedUs << " мкс" << std::endl;
}

void run_benchmark_barrier(const std::string& benchmarkName, Barrier& barrier) {
  std::cout << " Запуск: " << benchmarkName << " ... " << std::flush;

  auto time_start = std::chrono::high_resolution_clock::now();

  std::vector<std::thread> workerThreads;
  workerThreads.reserve(count_threads);
  for (int threadIndex = 0; threadIndex < count_threads; ++threadIndex) {
    workerThreads.emplace_back(race_with_barrier, std::ref(barrier),
                               count_iterations);
  }

  for (auto& th : workerThreads) {
    th.join();
  }

  auto time_end = std::chrono::high_resolution_clock::now();
  auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(
                       time_end - time_start)
                       .count();

  std::cout << "Время: " << elapsedUs << " мкс" << std::endl;
}
```

```
#pragma once
#include <mutex>
#include <random>
#include <string>
#include <thread>

#include "../sync/Barrier.hpp"
#include "../sync/CountingSemaphore.hpp"
#include "../sync/Monitor.hpp"
#include "../sync/SpinLock.hpp"
#include "../sync/SpinWait.hpp"
#include "../sync/SpinWaitMutex.hpp"

extern const int count_iterations;

void race_with_mutex(std::string& buffer, std::mutex& mtx);
void race_with_spinlock(std::string& buffer, SpinLock& lock);
void race_with_spinwait_mutex(std::string& buffer, SpinWaitMutex& lock);
void race_with_monitor(std::string& buffer, Monitor& mon);
void race_with_counting_semaphore(std::string& buffer, CountingSemaphore& sem);
void race_with_barrier(Barrier& barrier, int iterations);
```

```
#include "race_tasks.hpp"

extern const int count_iterations{100000};

// Pseudo-Random Number Generator
thread_local std::mt19937 prng{std::random_device{}()};
thread_local std::uniform_int_distribution<int> charDist(0, 127);

void race_with_mutex(std::string& buffer, std::mutex& mtx) {
  for (int iter = 0; iter < count_iterations; ++iter) {
    std::lock_guard<std::mutex> guard(mtx);
    buffer += static_cast<char>(charDist(prng));
  }
}

void race_with_spinlock(std::string& buffer, SpinLock& lock) {
  for (int iter = 0; iter < count_iterations; ++iter) {
    lock.lock();
    buffer += static_cast<char>(charDist(prng));
    lock.unlock();
  }
}

void race_with_spinwait_mutex(std::string& buffer, SpinWaitMutex& lock) {
  for (int iter = 0; iter < count_iterations; ++iter) {
    lock.lock();
    buffer += static_cast<char>(charDist(prng));
    lock.unlock();
  }
}

void race_with_monitor(std::string& buffer, Monitor& mon) {
  for (int iter = 0; iter < count_iterations; ++iter) {
    mon.enter();
    buffer += static_cast<char>(charDist(prng));
    mon.exit();
  }
}

void race_with_counting_semaphore(std::string& buffer, CountingSemaphore& sem) {
  for (int iter = 0; iter < count_iterations; ++iter) {
    sem.acquire();
    buffer += static_cast<char>(charDist(prng));
    sem.release();
  }
}

void race_with_barrier(Barrier& barrier, int iterations) {
  const int BATCH_SIZE = 1000;
  // синхронизация этапов
  for (int i = 0; i < iterations; i += BATCH_SIZE) {
    volatile int sink = 0;
    for (int j = 0; j < BATCH_SIZE; ++j) {
      sink += j;
    }
    barrier.arrive_and_wait();
  }
}
```

```
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
```

```
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
```

```
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
```

```
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
```

```
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
```

```
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
```
