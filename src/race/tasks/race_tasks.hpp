#pragma once
#include "../sync/SpinLock.hpp"
#include "../sync/SpinWait.hpp"
#include "../sync/SpinWaitMutex.hpp"

#include <mutex>
#include <random>
#include <string>
#include <thread>

extern const int count_iterations;

void race_with_mutex(std::string& buffer, std::mutex& mtx);
void race_with_spinlock(std::string& buffer, SpinLock& lock);
void race_with_spinwait_mutex(std::string& buffer, SpinWaitMutex& lock);