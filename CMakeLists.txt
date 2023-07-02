# SPDX-FileCopyrightText: 2022-2023 Denis Danilov
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.16)

project(soundscape LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WITH_CLANG_FORMAT "Enable clang-format check." OFF)
option(WITH_COVERAGE "Enable code coverage reports for tests (gcovr + gcc)." OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ClangFormat)
include(CodeCoverage)

set(WEB_SITE "https://github.com/ddanilov/soundscape")

set(APP_TITLE "Soundscape")
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(APP_NAME ${APP_TITLE})
else()
  string(TOLOWER ${APP_TITLE} APP_NAME)
endif()
message(STATUS "application name: ${APP_NAME}")

find_package(Git)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
)
string(STRIP ${GIT_VERSION} GIT_VERSION)
message(STATUS "git version: ${GIT_VERSION}")

set(APP_LIB ${APP_NAME}-lib)
set(APP_EXE ${APP_NAME})

add_subdirectory(src)

enable_testing(TRUE)
add_subdirectory(tests)
