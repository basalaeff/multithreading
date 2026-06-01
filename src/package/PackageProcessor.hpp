#include <algorithm>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// Сущность
struct Package {
  std::string code_;
  std::string city_;
  std::string full_name_;
};

// Класс-обработчик
class PackageProcessor {
 private:
  std::vector<Package> data_;
  std::string pattern_;
  std::regex regex_;
  size_t thread_count_;

  // Вспомогательная: извлекает первое слово (фамилию) из ФИО
  std::string extract_surname_(const std::string& full_name) const;

  // Вспомогательная: обработка по индексам [start_idx, end_idx)
  void process_chunk_(size_t start_idx, size_t end_idx,
                      std::vector<std::string>& local_results) const;

 public:
  PackageProcessor(size_t thread_count, const std::string& pattern)
      : pattern_(pattern), thread_count_(thread_count) {
    try {
      regex_ = std::regex(pattern, std::regex::ECMAScript);
    } catch (const std::regex_error& e) {
      std::cerr << "Ошибка регулярного выражения: " << e.what() << std::endl;
      std::exit(1);
    }
  }

  void load_data(const std::vector<Package>& data) { data_ = data; }

  // Последовательная обработка (1 поток)
  std::vector<std::string> run_sequential();

  // Многопоточная обработка (N потоков)
  std::vector<std::string> run_parallel();
};

// Вспомогательная: извлекает первое слово (фамилию) из ФИО
inline std::string PackageProcessor::extract_surname_(
    const std::string& full_name) const {
  std::istringstream stream(full_name);
  std::string surname;
  if (stream >> surname) {
    return surname;
  }
  return full_name;
}

// Вспомогательная: обработка по индексам [start_idx, end_idx)
inline void PackageProcessor::process_chunk_(
    size_t start_idx, size_t end_idx,
    std::vector<std::string>& local_results) const {
  for (size_t i = start_idx; i < end_idx; ++i) {
    if (std::regex_search(data_[i].code_, regex_)) {
      local_results.push_back(extract_surname_(data_[i].full_name_));
    }
  }
}

// Последовательная обработка (1 поток)
inline std::vector<std::string> PackageProcessor::run_sequential() {
  std::vector<std::string> results;
  // Вызываем с индексами: от 0 до конца массива
  process_chunk_(0, data_.size(), results);
  return results;
}

// Многопоточная обработка (N потоков)
inline std::vector<std::string> PackageProcessor::run_parallel() {
  if (thread_count_ == 0 || data_.empty()) return {};

  std::vector<std::vector<std::string>> thread_results(thread_count_);
  std::vector<std::thread> threads;
  threads.reserve(thread_count_);

  size_t total = data_.size();
  size_t chunk_size = total / thread_count_;
  size_t remainder = total % thread_count_;
  size_t start_index = 0;

  for (size_t i = 0; i < thread_count_; ++i) {
    size_t current_chunk_size = chunk_size + (i < remainder ? 1 : 0);
    if (current_chunk_size == 0) break;

    size_t end_index = start_index + current_chunk_size;

    // Передаём индексы, а не копию вектора
    threads.emplace_back(&PackageProcessor::process_chunk_, this, start_index,
                         end_index, std::ref(thread_results[i]));

    start_index = end_index;
  }

  for (auto& t : threads) {
    t.join();
  }

  std::vector<std::string> final_results;
  for (const auto& res : thread_results) {
    final_results.insert(final_results.end(), res.begin(), res.end());
  }
  return final_results;
}

// Загружает данные из CSV-файла
bool load_data_from_file(const std::string& filename,
                         std::vector<Package>& out_data);