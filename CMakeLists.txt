cmake_minimum_required(VERSION 3.24)
project(Multithreading LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# ====================== Main ======================
find_package(Threads REQUIRED)

add_executable(race src/race/main.cpp src/race/tasks/race_tasks.cpp)
add_executable(package src/package/main.cpp src/package/load_data_from_file.cpp)
add_executable(banker src/banker/main.cpp src/banker/BankerAlgorithm.cpp)

target_include_directories(race PRIVATE src)
target_link_libraries(race Threads::Threads)
target_include_directories(package PRIVATE src)

# ====================== Formatting ======================

file(GLOB_RECURSE FORMAT_FILES
  "src/*.cpp" "src/*.hpp"
)

add_custom_target(format
    COMMAND clang-format -i ${FORMAT_FILES}
    COMMENT "Formatting code"
)