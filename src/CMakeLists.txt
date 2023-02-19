find_package(Qt6 6.3 COMPONENTS Multimedia Widgets REQUIRED)
message(STATUS "Using Qt ${Qt6_VERSION} from ${Qt6_DIR}")

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

set(HEADERS
  IconLabel.h
  JsonRW.h
  MainWindow.h
  Player.h
  PositionLabel.h
  PositionSlider.h
  Status.h
  Track.h
  TrackControls.h
  TrackSettings.h
  Transition.h
  TransitionIcon.h
  Volume.h
)

set(SOURCES
  IconLabel.cpp
  JsonRW.cpp
  Main.cpp
  MainWindow.cpp
  Player.cpp
  PositionLabel.cpp
  PositionSlider.cpp
  Status.cpp
  Track.cpp
  TrackControls.cpp
  TrackSettings.cpp
  Transition.cpp
  TransitionIcon.cpp
  Volume.cpp
)

set(RESOURCES
  ${PROJECT_SOURCE_DIR}/icons/icons.qrc
  ${PROJECT_SOURCE_DIR}/icons/win32.rc
  ${PROJECT_SOURCE_DIR}/styles/styles.qrc
)

qt_add_library(${APP_LIB} STATIC
  ${HEADERS}
  ${SOURCES}
)
target_link_libraries(${APP_LIB} Qt::Multimedia Qt::Widgets)

qt_add_executable(${APP_EXE} WIN32 ${RESOURCES})
target_link_libraries(${APP_EXE} PRIVATE ${APP_LIB})

if(WIN32)
  install(TARGETS ${APP_EXE} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  qt_generate_deploy_app_script(TARGET ${APP_EXE} FILENAME_VARIABLE deploy_script)
  install(SCRIPT ${deploy_script})
endif(WIN32)
