cmake_minimum_required(VERSION 3.10)
project(aka)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(SQLite3 REQUIRED sqlite3)

include_directories(${GTK3_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})

set(SOURCES main.c gui.c database.c)

add_executable(aka ${SOURCES})

target_link_libraries(aka ${GTK3_LIBRARIES} ${SQLite3_LIBRARIES})

target_compile_options(aka PRIVATE ${GTK3_CFLAGS_OTHER} -Wall -Wextra)
