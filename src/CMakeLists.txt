find_package(Qt6 COMPONENTS Multimedia Widgets REQUIRED)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

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
  Volume.cpp
)

set(RESOURCES
  ${PROJECT_SOURCE_DIR}/icons/icons.qrc
  ${PROJECT_SOURCE_DIR}/styles/styles.qrc
)

add_library(${APP_LIB} STATIC
  ${HEADERS}
  ${SOURCES}
)
target_link_libraries(${APP_LIB} Qt::Multimedia Qt::Widgets)

add_executable(${APP_EXE} ${RESOURCES})
target_link_libraries(${APP_EXE} PRIVATE ${APP_LIB})
