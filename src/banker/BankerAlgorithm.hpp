#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>

class BankerAlgorithm {
 private:
  int numProcesses_;
  int numResources_;
  std::vector<std::vector<int>> allocation_;
  std::vector<std::vector<int>> max_;
  std::vector<int> available_;
  std::vector<std::vector<int>> need_;

  void computeNeed();
  bool checkSafety(std::vector<int>& safeOrder) const;

 public:
  BankerAlgorithm(int processes, int resources,
                  const std::vector<std::vector<int>>& alloc,
                  const std::vector<std::vector<int>>& maxReq,
                  const std::vector<int>& avail);

  bool isSafe(std::vector<int>& safeOrder);
  bool requestResources(int pid, const std::vector<int>& request);

  // Геттеры
  const std::vector<std::vector<int>>& getAllocation() const {
    return allocation_;
  }
  const std::vector<std::vector<int>>& getMax() const { return max_; }
  const std::vector<int>& getAvailable() const { return available_; }
  const std::vector<std::vector<int>>& getNeed() const { return need_; }
};