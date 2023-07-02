# SPDX-FileCopyrightText: 2022 Denis Danilov
# SPDX-License-Identifier: GPL-3.0-only

# let's use CPPLINT property to run clang-format check
find_program(CLANG_FORMAT clang-format)
if(WITH_CLANG_FORMAT AND CLANG_FORMAT)
  message(STATUS "Setup clang-format check")
  set(CMAKE_CXX_CPPLINT "${CLANG_FORMAT};--dry-run;--Werror")
endif()
