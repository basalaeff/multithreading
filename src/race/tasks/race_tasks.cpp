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