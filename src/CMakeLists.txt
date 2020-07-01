set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.hpp *.cpp)
set(SOURCES ${SOURCES})

add_executable(${BINARY}_run ${SOURCES})
add_library(${BINARY}_lib ${SOURCES})

if(Boost_FOUND)
    target_link_libraries(${BINARY}_run ${Boost_LIBRARIES})
    message("-- Boost link to: ${Boost_VERSION_STRING}")
else()
    message(FATAL_ERROR "Boost not found!")
endif()

