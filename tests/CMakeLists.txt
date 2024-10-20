# SPDX-FileCopyrightText: 2022-2024 Denis Danilov
# SPDX-License-Identifier: GPL-3.0-only

enable_testing(TRUE)

include_directories(${CMAKE_SOURCE_DIR}/src)

macro(unit_test SOURCE_FILE_NAME)
  get_filename_component(TESTNAME ${SOURCE_FILE_NAME} NAME_WE)
  add_executable(${TESTNAME} ${SOURCE_FILE_NAME} ${RESOURCES})
  qt_add_resources(${TESTNAME} media_resources
    PREFIX "/media"
    BASE "media"
    FILES
      media/sound_0000.wav
      media/sound_0100.wav
      media/sound_XXXX.wav
      media/video_0100.webm
  )
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
unit_test(TestTransition.cpp)
