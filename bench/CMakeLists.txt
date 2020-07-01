set(BINARY ${CMAKE_PROJECT_NAME}_bench)

file(GLOB_RECURSE BENCH_SOURCES LIST_DIRECTORIES false *.hpp *.cpp)

set(SOURCES ${BENCH_SOURCES})

# the benchmark suite only works if google benchmark is installed
# https://github.com/google/benchmark
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(${BINARY} ${BENCH_SOURCES})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib benchmark::benchmark)
