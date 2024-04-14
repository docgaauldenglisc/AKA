cmake_minimum_required(VERSION 3.10)
project(aka)

set(ORIG_DIR "${CMAKE_SOURCE_DIR}/help")
set(DEST_DIR "${CMAKE_BINARY_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

file(COPY ${ORIG_DIR} DESTINATION ${DEST_DIR})

find_package(PkgConfig REQUIRED)
find_package(PCRE REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(SQLite3 REQUIRED sqlite3)

include_directories(${GTK3_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${PCRE_INCLUDE_DIRS})

set(SOURCES main.c gui.c database.c rgx.c help.c)

add_executable(aka ${SOURCES})

target_link_libraries(aka ${GTK3_LIBRARIES} ${SQLite3_LIBRARIES} ${PCRE_LIBRARIES})

target_compile_options(aka PRIVATE ${GTK3_CFLAGS_OTHER} -Wall -Wextra)
