#include "run.hpp"

int main() {
  std::cout << "Число потоков: " << count_threads
            << ", Итераций на поток: " << count_iterations << "\n\n";

  {
    std::mutex stdMutex;
    run_benchmark("std::mutex", race_with_mutex, stdMutex);
  }

  return 0;
}