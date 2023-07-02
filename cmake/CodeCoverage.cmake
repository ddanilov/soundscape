# SPDX-FileCopyrightText: 2022 Denis Danilov
# SPDX-License-Identifier: GPL-3.0-only

find_program(GCOVR gcovr)

set(GCOVR_COVERAGE_ENABLED OFF)

if(WITH_COVERAGE AND GCOVR AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "setup GCOV coverage report")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(GCOVR_COVERAGE_ENABLED ON)
  set(GCOVR_REPORT_BASE "coverage")
  message(STATUS "coverage report base: ${PROJECT_BINARY_DIR}/${GCOVR_REPORT_BASE}")
endif()
