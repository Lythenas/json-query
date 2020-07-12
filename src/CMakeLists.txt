set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.hpp *.cpp)
set(SOURCES ${SOURCES})

FetchContent_Declare(tclap
    GIT_REPOSITORY https://github.com/mirror/tclap)
FetchContent_GetProperties(tclap)
FetchContent_Populate(tclap)

include_directories("${tclap_SOURCE_DIR}/include")

add_executable(${BINARY}_run ${SOURCES})
add_library(${BINARY}_lib ${SOURCES})

target_link_libraries(${BINARY}_run PUBLIC ${Boost_LIBRARIES})

