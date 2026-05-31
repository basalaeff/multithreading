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

  return 0;
}