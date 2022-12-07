cmake_minimum_required(VERSION 3.20)
project(scorewarrior_test)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost)
if (${Boost_FOUND})
    message(STATUS "Boost version: ${Boost_VERSION}")
    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost library dir: ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost libs: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "Boost was not found")
endif()

file(GLOB SRCS src/*.cpp src/events/*.cpp src/creatures/*.cpp)

add_executable(${PROJECT_NAME}
        ${SRCS}
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        ${Boost_INCLUDE_DIRS})
