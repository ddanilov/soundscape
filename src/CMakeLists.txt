find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(HEADERS
  JsonRW.h
  MainWindow.h
  Track.h
  TrackControls.h
  Volume.h
)

set(SOURCES
  JsonRW.cpp
  Main.cpp
  MainWindow.cpp
  Track.cpp
  TrackControls.cpp
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
target_link_libraries(${APP_LIB} Qt::Widgets)

add_executable(${APP_EXE} ${RESOURCES})
target_link_libraries(${APP_EXE} PRIVATE ${APP_LIB})
