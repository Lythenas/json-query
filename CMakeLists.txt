cmake_minimum_required(VERSION 3.10)

project(jsonquery VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include(FetchContent)

find_package(Boost 1.72 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(bench)

