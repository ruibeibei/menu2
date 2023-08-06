cmake_minimum_required(VERSION 3.16)

project(menu CXX)


# Search for the libncurses library
set(NCURSES_ROOT "/usr/lib/x86_64-linux-gnu/" CACHE PATH "Path to ncurses root directory")
find_library(NCURSES_LIBRARY ncurses ${NCURSES_ROOT})

if(NCURSES_LIBRARY)
    message(STATUS "libncurses found: ${NCURSES_LIBRARY}")
else()
    message(FATAL_ERROR "libncurses not found")
endif()

include_directories(.)

#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wpedantic -Werror -O0 -fno-elide-constructors")

add_executable(step1 step1/menu.cpp)
target_link_libraries(step1 ${NCURSES_LIBRARY})


