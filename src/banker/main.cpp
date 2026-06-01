#include <iostream>
#include <vector>

#include "BankerAlgorithm.hpp"

using namespace std;

int main() {
  int procCount, resTypeCount;

  cout << "Введите количество процессов: ";
  cin >> procCount;
  cout << "Введите количество типов ресурсов: ";
  cin >> resTypeCount;

  vector<vector<int>> allocatedMatrix(procCount, vector<int>(resTypeCount));
  vector<vector<int>> maxMatrix(procCount, vector<int>(resTypeCount));
  vector<int> availableVector(resTypeCount);

  cout << "Введите матрицу выделения (" << procCount << "x" << resTypeCount
       << "):\n";
  for (int p = 0; p < procCount; ++p)
    for (int r = 0; r < resTypeCount; ++r) cin >> allocatedMatrix[p][r];

  cout << "Введите матрицу максимальных потребностей (" << procCount << "x"
       << resTypeCount << "):\n";
  for (int p = 0; p < procCount; ++p)
    for (int r = 0; r < resTypeCount; ++r) cin >> maxMatrix[p][r];

  cout << "Введите вектор доступных ресурсов (" << resTypeCount
       << " элементов):\n";
  for (int r = 0; r < resTypeCount; ++r) cin >> availableVector[r];

  try {
    // Создаём объект класса вместо вызова свободных функций
    BankerAlgorithm banker(procCount, resTypeCount, allocatedMatrix, maxMatrix,
                           availableVector);

    vector<int> safeOrder;
    if (banker.isSafe(safeOrder)) {
      cout << "\nСистема находится в БЕЗОПАСНОМ состоянии.\nБезопасная "
              "последовательность: ";
      for (size_t i = 0; i < safeOrder.size(); ++i) {
        cout << "P" << safeOrder[i];
        if (i != safeOrder.size() - 1) cout << " -> ";
      }
      cout << "\n\n";
    } else {
      cout << "\nСистема находится в НЕБЕЗОПАСНОМ состоянии! Возможна "
              "блокировка.\n\n";
      return 0;
    }

    int requesterPid;
    cout << "Введите ID процесса, запрашивающего ресурсы: ";
    cin >> requesterPid;

    vector<int> requestVector(resTypeCount);
    cout << "Введите вектор запроса (" << resTypeCount << " элементов): ";
    for (int r = 0; r < resTypeCount; ++r) cin >> requestVector[r];

    // Вызываем метод объекта
    banker.requestResources(requesterPid, requestVector);

  } catch (const exception& e) {
    cerr << "Ошибка инициализации: " << e.what() << "\n";
    return 1;
  }

  return 0;
}