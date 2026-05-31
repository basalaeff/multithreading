#include "race_tasks.hpp"

extern const int count_iterations{1000};

// Pseudo-Random Number Generator
thread_local std::mt19937 prng{std::random_device{}()};
thread_local std::uniform_int_distribution<int> charDist(0, 127);

void race_with_mutex(std::string& buffer, std::mutex& mtx) {
  for (int iter = 0; iter < count_iterations; ++iter) {
    std::lock_guard<std::mutex> guard(mtx);
    buffer += static_cast<char>(charDist(prng));
  }
}

void race_with_spinlock(std::string& buffer, SpinLock& lock)
{
    for (int iter = 0; iter < count_iterations; ++iter) {
        lock.lock();
        buffer += static_cast<char>(charDist(prng));
        lock.unlock();
    }
}

void race_with_spinwait_mutex(std::string& buffer, SpinWaitMutex& lock)
{
    for (int iter = 0; iter < count_iterations; ++iter) {
        lock.lock();
        buffer += static_cast<char>(charDist(prng));
        lock.unlock();
    }
}

void race_with_monitor(std::string& buffer, Monitor& mon)
{
    for (int iter = 0; iter < count_iterations; ++iter) {
        mon.enter();
        buffer += static_cast<char>(charDist(prng));
        mon.exit();
    }
}