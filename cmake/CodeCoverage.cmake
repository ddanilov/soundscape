find_program(GCOVR gcovr)

if(GCOVR AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "setup GCOV coverage report")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fprofile-arcs --coverage -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

  set(CTEST_COVERAGE ${PROJECT_NAME}_ctest_coverage)
  add_CUSTOM_TARGET(${CTEST_COVERAGE}
    COMMAND ctest -C $<CONFIG> $ENV{ARGS} $$ARGS
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

  set(REPORT_BASE "coverage")
  message(STATUS "${PROJECT_BINARY_DIR}/${REPORT_BASE}")
  add_custom_command(TARGET ${CTEST_COVERAGE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${REPORT_BASE}
    COMMAND gcovr
      --html --html-details
      --output ${REPORT_BASE}/index.html
      --exclude-unreachable-branches
      --exclude-throw-branches
      --root ${PROJECT_SOURCE_DIR}
      --object-directory ${PROJECT_BINARY_DIR}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "code coverage report by gcovr")
endif()
