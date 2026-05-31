#pragma once

#include <mutex>
#include <random>
#include <string>
#include <thread>

extern const int count_iterations;

void race_with_mutex(std::string& buffer, std::mutex& mtx);

