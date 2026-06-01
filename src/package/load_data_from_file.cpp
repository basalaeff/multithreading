#include <fstream>

#include "PackageProcessor.hpp"
// Загружает данные из CSV-файла
bool load_data_from_file(const std::string& filename,
                         std::vector<Package>& out_data) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "❌ Не удалось открыть файл: " << filename << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) continue;  // Пропускаем пустые строки

    std::istringstream ss(line);
    std::string code, city, full_name;

    // code, city, full_name
    if (std::getline(ss, code, ',') && std::getline(ss, city, ',') &&
        std::getline(ss, full_name)) {  // До конца строки

      // Убираем возможные пробелы/символы \r
      if (!code.empty() && code.back() == '\r') code.pop_back();
      if (!city.empty() && city.back() == '\r') city.pop_back();
      if (!full_name.empty() && full_name.back() == '\r') full_name.pop_back();

      out_data.push_back({code, city, full_name});
    }
  }
  return !out_data.empty();
}