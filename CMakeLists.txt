cmake_minimum_required(VERSION 3.10)

project(jsonquery VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Winconsistent-missing-override)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(Boost 1.72)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(jsonquery
    src/jsonquery.cpp)

