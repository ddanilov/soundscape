cmake_minimum_required(VERSION 3.16)

project(soundscape-qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ClangFormat OPTIONAL)
include(CodeCoverage OPTIONAL)

set(APP_LIB ${PROJECT_NAME}_lib)
set(APP_EXE ${PROJECT_NAME})

add_subdirectory(src)

enable_testing(TRUE)
add_subdirectory(tests)
