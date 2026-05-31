#pragma once
#include "../sync/SpinLock.hpp"
#include "../sync/SpinWait.hpp"
#include "../sync/SpinWaitMutex.hpp"
#include "../sync/Monitor.hpp"
#include "../sync/CountingSemaphore.hpp"
#include "../sync/Barrier.hpp"

#include <mutex>
#include <random>
#include <string>
#include <thread>

extern const int count_iterations;

void race_with_mutex(std::string& buffer, std::mutex& mtx);
void race_with_spinlock(std::string& buffer, SpinLock& lock);
void race_with_spinwait_mutex(std::string& buffer, SpinWaitMutex& lock);
void race_with_monitor(std::string& buffer, Monitor& mon);
void race_with_counting_semaphore(std::string& buffer, CountingSemaphore& sem);
void race_with_barrier(Barrier& barrier, int iterations);