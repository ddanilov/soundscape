# let's use CPPLINT property to run clang-format check
find_program(CLANGFORMAT clang-format)
if(CLANGFORMAT)
  message(STATUS "Setup clang-format check")
  set(CMAKE_CXX_CPPLINT "${CLANGFORMAT};--dry-run;--Werror")
endif()
