# SPDX-FileCopyrightText: 2022-2024 Denis Danilov
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.16)

project(soundscape LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WITH_CLANG_FORMAT "Enable clang-format check." OFF)
option(WITH_DEPLOY_SCRIPT "Enable Qt deploy script." OFF)

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

if(NOT DEFINED APP_VERSION)
  find_package(Git)
  if(Git_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      OUTPUT_VARIABLE APP_VERSION)
  elseif()
    set(APP_VERSION "unknown")
  endif()
endif()
string(STRIP ${APP_VERSION} APP_VERSION)
message(STATUS "version: ${APP_VERSION}")

set(APP_LIB ${APP_NAME}-lib)
set(APP_EXE ${APP_NAME})

find_package(Qt6 6.2 COMPONENTS
  LinguistTools
  Multimedia
  Test
  Widgets
  REQUIRED)
message(STATUS "Using Qt ${Qt6_VERSION} from ${Qt6_DIR}")

if(Qt6_VERSION VERSION_GREATER_EQUAL 6.3)
  qt_standard_project_setup()
else()
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  include(GNUInstallDirs)
endif()
set(CMAKE_AUTORCC ON)

add_subdirectory(src)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_subdirectory(linux)
endif()

add_subdirectory(tests)
