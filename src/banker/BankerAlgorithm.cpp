#include "BankerAlgorithm.hpp"

using namespace std;

BankerAlgorithm::BankerAlgorithm(int processes, int resources,
                                 const vector<vector<int>>& alloc,
                                 const vector<vector<int>>& maxReq,
                                 const vector<int>& avail)
    : numProcesses_(processes),
      numResources_(resources),
      allocation_(alloc),
      max_(maxReq),
      available_(avail) {

  if (numProcesses_ <= 0 || numResources_ <= 0) {
    throw invalid_argument("Количество процессов и ресурсов должно быть > 0.");
  }

  if (allocation_.size() != static_cast<size_t>(numProcesses_) ||
      max_.size() != static_cast<size_t>(numProcesses_) ||
      available_.size() != static_cast<size_t>(numResources_)) {
    throw invalid_argument("Размеры входных данных не совпадают с указанными.");
  }

  computeNeed();
}

void BankerAlgorithm::computeNeed()
{
  need_.assign(numProcesses_, vector<int>(numResources_));

  for (int p = 0; p < numProcesses_; ++p) {
    for (int r = 0; r < numResources_; ++r) {
      if (max_[p][r] < allocation_[p][r]) {
        throw invalid_argument("Max не может быть меньше Allocation.");
      }

      need_[p][r] = max_[p][r] - allocation_[p][r];
    }
  }
}

bool BankerAlgorithm::checkSafety(vector<int>& safeOrder) const
{
  vector<int> work = available_;
  vector<bool> finished(numProcesses_, false);
  safeOrder.clear();
  int finishedCount = 0;

  while (finishedCount < numProcesses_) {
    bool progressed = false;
    for (int p = 0; p < numProcesses_; ++p) {
      if (finished[p]) continue;

      bool canRun = true;
      for (int r = 0; r < numResources_; ++r) {
        if (need_[p][r] > work[r]) {
          canRun = false;
          break;
        }
      }

      if (canRun) {
        for (int r = 0; r < numResources_; ++r) {
          work[r] += allocation_[p][r];
        }
        safeOrder.push_back(p);
        finished[p] = true;
        progressed = true;
        ++finishedCount;
      }
    }
    if (!progressed) return false;
  }
  return true;
}

bool BankerAlgorithm::isSafe(vector<int>& safeOrder) {
  return checkSafety(safeOrder);
}

bool BankerAlgorithm::requestResources(int pid, const vector<int>& request) {
  if (pid < 0 || pid >= numProcesses_) {
    cerr << "Ошибка: Некорректный ID процесса.\n";
    return false;
  }

  if (request.size() != static_cast<size_t>(numResources_)) {
    cerr << "Ошибка: Размер вектора запроса не совпадает с количеством типов "
            "ресурсов.\n";
    return false;
  }

  // Проверка Need
  for (int r = 0; r < numResources_; ++r) {
    if (request[r] > need_[pid][r]) {
      cerr << "Ошибка: Запрос превышает максимальную заявку!\n";
      return false;
    }
  }

  // Проверка Available
  for (int r = 0; r < numResources_; ++r) {
    if (request[r] > available_[r]) {
      cout << "Процесс должен ждать: ресурсы недоступны.\n";
      return false;
    }
  }

  // Предварительное выделение
  for (int r = 0; r < numResources_; ++r) {
    available_[r] -= request[r];
    allocation_[pid][r] += request[r];
    need_[pid][r] -= request[r];
  }

  // Проверка безопасности
  vector<int> tempOrder;
  if (checkSafety(tempOrder)) {
    cout << "Запрос удовлетворён. Система остаётся в безопасном состоянии.\n";
    return true;
  }

  // Откат
  for (int r = 0; r < numResources_; ++r) {
    available_[r] += request[r];
    allocation_[pid][r] -= request[r];
    need_[pid][r] += request[r];
  }

  cout << "Запрос отклонён: приведёт к небезопасному состоянию.\n";
  return false;
}