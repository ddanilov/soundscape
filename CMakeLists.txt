cmake_minimum_required(VERSION 3.16)

project(soundscape-qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WITH_CLANG_FORMAT "Enable clang-format check." OFF)
option(WITH_COVERAGE "Enable code coverage reports for tests (gcovr + gcc)." OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ClangFormat)
include(CodeCoverage)

set(APP_LIB ${PROJECT_NAME}_lib)
set(APP_EXE ${PROJECT_NAME})

add_subdirectory(src)

enable_testing(TRUE)
add_subdirectory(tests)

if(WIN32)
  get_target_property(_qmake_executable Qt::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

  message(STATUS "windeploy: ${WINDEPLOYQT_EXECUTABLE}")
  message(STATUS "install prefix: ${CMAKE_INSTALL_PREFIX}")

  set(CMAKE_INSTALL_BINDIR "")
  message(STATUS "bin dir: ${CMAKE_INSTALL_BINDIR}")

  set(APP_TARGET_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
  message(STATUS "target  dir: ${APP_TARGET_DIR}")

  install(FILES README.md DESTINATION ${APP_TARGET_DIR})
  install(FILES LICENSE DESTINATION ${APP_TARGET_DIR})
  install(TARGETS ${APP_EXE} DESTINATION ${APP_TARGET_DIR})
  install(CODE "execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --compiler-runtime \"${APP_TARGET_DIR}\")")
endif(WIN32)
