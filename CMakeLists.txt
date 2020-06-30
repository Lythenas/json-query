cmake_minimum_required(VERSION 3.10)

project(jsonquery VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(Boost 1.72 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(jsonquery
    src/jsonquery.cpp)

#### BENCHMARKS ####

# include(FetchContent)
# set(BENCHMARK_ENABLE_TESTING OFF)
# FetchContent_Declare(googlebenchmark
#     GIT_REPOSITORY https://github.com/google/benchmark)
# FetchContent_MakeAvailable(googlebenchmark)
#
#
# # the benchmark suite only works if goole benchmark is installed
# # https://github.com/google/benchmark
# add_executable(jsonquery_bench
#     bench/bench.cpp)
#
# target_link_libraries(jsonquery_bench benchmark::benchmark)
