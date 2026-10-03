# SPDX-FileCopyrightText: 2024-2026 Denis Danilov
# SPDX-License-Identifier: GPL-3.0-only

option(WITH_COVERAGE "Enable code coverage reports for tests." OFF)

if(WITH_COVERAGE)
  find_program(GCOVR gcovr REQUIRED)

  if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug"))
    message(SEND_ERROR "can't prepare coverage")
  endif()

  add_compile_options(--coverage)
  add_link_options(--coverage)

  set(CTEST_COVERAGE_DIR "coverage")
  message(STATUS "coverage report base: ${PROJECT_BINARY_DIR}/${CTEST_COVERAGE_DIR}")

  set(EXCLUDE_PATTERN ".*tests")
  set(EXCLUDE_PATTERN_RC ".*rcc")
  set(EXCLUDE_PATTERN_AUTOGEN ".*lib_autogen")

  add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${CTEST_COVERAGE_DIR}
    COMMAND gcovr
    --html --html-details
    --exclude "${EXCLUDE_PATTERN}"
    --exclude-directory "${EXCLUDE_PATTERN_RC}"
    --exclude-directory "${EXCLUDE_PATTERN_AUTOGEN}"
    --output ${CTEST_COVERAGE_DIR}/index.html
    --root ${PROJECT_SOURCE_DIR}
    --object-directory ${PROJECT_BINARY_DIR}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

  add_custom_target(coveralls
    COMMAND gcovr
    --coveralls coverage.json
    --exclude "${EXCLUDE_PATTERN}"
    --exclude-directory "${EXCLUDE_PATTERN_RC}"
    --exclude-directory "${EXCLUDE_PATTERN_AUTOGEN}"
    --root ${PROJECT_SOURCE_DIR}
    --object-directory ${PROJECT_BINARY_DIR}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

endif()
