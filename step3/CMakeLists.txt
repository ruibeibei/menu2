# Search for the libncurses library
set(NCURSES_ROOT "/usr/lib/aarch64-linux-gnu" CACHE PATH "Path to ncurses root directory")
find_library(NCURSES_LIBRARY ncurses ${NCURSES_ROOT})

if(NCURSES_LIBRARY)
    message(STATUS "libncurses found: ${NCURSES_LIBRARY}")
else()
    message(FATAL_ERROR "libncurses not found")
endif()

include_directories(${MMPRJ_INCLUDE_DIRS})

#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wpedantic -Werror -O0 -fno-elide-constructors")

add_executable(menu_2 main.cpp KeyHandlerStrategy.cpp MenuInterface.cpp MultiLevelMenu.cpp) 
target_link_libraries(menu_2 ${NCURSES_LIBRARY})
