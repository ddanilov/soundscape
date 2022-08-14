find_package(Qt6 COMPONENTS Test REQUIRED)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(RESOURCES ./media/media.qrc)

include_directories(${CMAKE_SOURCE_DIR}/src)

macro(unit_test SOURCE_FILE_NAME)
  get_filename_component(TESTNAME ${SOURCE_FILE_NAME} NAME_WE)
  add_executable(${TESTNAME} ${SOURCE_FILE_NAME} ${RESOURCES})
  target_link_libraries(${TESTNAME} PRIVATE ${APP_LIB} Qt::Test)
  add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endmacro(unit_test)

unit_test(TestJsonRW.cpp)
unit_test(TestMainWindow.cpp)
unit_test(TestPlayer.cpp)
unit_test(TestPositionLabel.cpp)
unit_test(TestTrack.cpp)
unit_test(TestTrackControls.cpp)
unit_test(TestTrackSettings.cpp)

if(GCOVR_COVERAGE_ENABLED)
  set(CTEST_COVERAGE_TARGET ctest_coverage)
  set(CTEST_COVERAGE_DIR ${GCOVR_REPORT_BASE}/${CTEST_COVERAGE_TARGET})
  add_CUSTOM_TARGET(${CTEST_COVERAGE_TARGET}
    COMMAND ctest -C $<CONFIG> $ENV{ARGS} $$ARGS
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  add_custom_command(TARGET ${CTEST_COVERAGE_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${CTEST_COVERAGE_DIR}
    COMMAND gcovr
      --html --html-details
      --output ${CTEST_COVERAGE_DIR}/index.html
      --exclude-unreachable-branches
      --exclude-throw-branches
      --root ${PROJECT_SOURCE_DIR}
      --object-directory ${PROJECT_BINARY_DIR}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "code coverage report by gcovr")
endif()
