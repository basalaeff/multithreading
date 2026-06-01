#include "PackageProcessor.hpp"

int main() {
  const size_t DATA_SIZE = 500'000;
  const size_t THREAD_COUNT = 4;
  const std::string PATTERN = "^[A-Z]{3}-\\d{4}$";

  std::cout << "Подготовка данных (" << DATA_SIZE << " записей)...\n";
  std::string filename{"../src/package/db.csv"};
  std::vector<Package> packages;
  load_data_from_file(filename, packages);

  PackageProcessor processor(THREAD_COUNT, PATTERN);
  processor.load_data(packages);

  // ЗАМЕР 1: Последовательно
  std::cout << "Запуск последовательной обработки...\n";
  auto start_seq = std::chrono::high_resolution_clock::now();
  auto results_seq = processor.run_sequential();
  auto end_seq = std::chrono::high_resolution_clock::now();
  auto time_seq =
      std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq)
          .count();

  // ЗАМЕР 2: Параллельно
  std::cout << "Запуск многопоточной обработки (" << THREAD_COUNT
            << " потоков)...\n";
  auto start_par = std::chrono::high_resolution_clock::now();
  auto results_par = processor.run_parallel();
  auto end_par = std::chrono::high_resolution_clock::now();
  auto time_par =
      std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par)
          .count();

  // ВЫВОД
  std::cout << "\nИтоги:\n";
  std::cout << "Время (1 поток): " << time_seq << " мс\n";
  std::cout << "Время (" << THREAD_COUNT << " потока): " << time_par << " мс\n";

  if (time_par > 0) {
    std::cout << "Ускорение: " << std::fixed
              << std::setprecision(2)
              << (static_cast<double>(time_seq) / time_par) << "x\n";
  }

  std::cout << "Найдено совпадений: " << results_seq.size() << "\n";

  // Проверка корректности (сравниваем результаты)
  if (results_seq == results_par) {
    std::cout << "Результаты идентичны\n";
  } else {
    std::cout << "ОШИБКА: результаты различаются!\n";
  }

  std::cout << "\nРезультаты:\n";
  for (size_t i = 0; i < results_seq.size(); ++i) {
    std::cout << " - " << results_seq[i] << "\n";
  }

  return 0;
}