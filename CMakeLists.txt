cmake_minimum_required(VERSION 3.5)
project(cjsh-configure VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)
include_directories(include ${CURSES_INCLUDE_DIR})

add_executable(cjsh-configure
    src/main.cpp
    src/tui_configurator.cpp
    src/cjsh_filesystem.cpp
    include/tui_configurator.h
    include/cjsh_filesystem.h
)

target_link_libraries(cjsh-configure PRIVATE ${CURSES_LIBRARIES})
