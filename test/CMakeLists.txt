set(BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true *.hpp *.cpp)
# fuzzer test ist configured separately below
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test_fuzzer.cpp")

set(SOURCES ${TEST_SOURCES})

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2)
FetchContent_MakeAvailable(Catch2)

# normal tests
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib Catch2)

# fuzzer test
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_executable(${BINARY}_fuzz "test_fuzzer.cpp")
    target_compile_options(${BINARY}_fuzz
        PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O1 -fsanitize=fuzzer,address -fprofile-instr-generate -fcoverage-mapping >
        )
    target_link_libraries(${BINARY}_fuzz
        PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer,address>
        PUBLIC ${CMAKE_PROJECT_NAME}_lib
        )
endif()
