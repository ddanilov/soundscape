# let's use CPPLINT property to run clang-format check
FIND_PROGRAM(CLANGFORMAT clang-format)
IF(CLANGFORMAT)
  MESSAGE(STATUS "Setup clang-format check")
  SET(CMAKE_CXX_CPPLINT "${CLANGFORMAT};--dry-run;--Werror")
ENDIF()
