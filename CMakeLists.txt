cmake_minimum_required(VERSION 3.24)
project(Multithreading LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# ====================== Main ======================
find_package(Threads REQUIRED)

add_executable(race src/race/main.cpp src/race/tasks/race_tasks.cpp)

target_include_directories(race PRIVATE src)
target_link_libraries(race Threads::Threads)

# ====================== Formatting ======================

file(GLOB_RECURSE FORMAT_FILES
  "src/*.cpp" "src/*.hpp"
)

add_custom_target(format
    COMMAND clang-format -i ${FORMAT_FILES}
    COMMENT "Formatting code"
)