find_package(Qt6 COMPONENTS Test REQUIRED)

macro(unit_test SOURCE_FILE_NAME)
  get_filename_component(TESTNAME ${SOURCE_FILE_NAME} NAME_WE)
  add_executable(${TESTNAME} ${SOURCE_FILE_NAME})
  target_link_libraries(${TESTNAME} PRIVATE ${APP_LIB} Qt::Test)
  add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endmacro(unit_test)

set(CMAKE_AUTOMOC ON)
include_directories(${CMAKE_SOURCE_DIR}/src)

unit_test(TestJsonRW.cpp)
unit_test(TestMainWindow.cpp)
