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