CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

PROJECT(soundscape-qt LANGUAGES CXX)

# let's use CPPLINT property to run clang-format check
find_program(CLANGFORMAT clang-format)
if (CLANGFORMAT)
  set(CMAKE_CXX_CPPLINT "${CLANGFORMAT};--dry-run;--Werror")
endif ()

ADD_SUBDIRECTORY(src)
