cmake_minimum_required(VERSION 3.20)
project(scorewarrior_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

option(SANITIZER_ANALYSIS "Sanitizers analysis" OFF)

if (SANITIZER_ANALYSIS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -O0 \
        -g \
        -fsanitize=thread \
    ")
endif()

#-fsanitize=address \
#-fsanitize=leak \
#-fsanitize=thread \
#-fsanitize=undefined \
#-fsanitize=memory

find_package(Boost)
if (${Boost_FOUND})
    message(STATUS "Boost version: ${Boost_VERSION}")
    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost library dir: ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost libs: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "Boost was not found")
endif()

find_package(Boost
    COMPONENTS
        thread
     REQUIRED)

file(GLOB_RECURSE SRCS src/*.cpp)

add_executable(${PROJECT_NAME}
        ${SRCS}
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        ${Boost_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME} PUBLIC
        Boost::thread
        )
