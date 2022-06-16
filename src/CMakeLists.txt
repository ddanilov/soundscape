FIND_PACKAGE(Qt6 COMPONENTS Widgets REQUIRED)

SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOMOC ON)

SET(HEADERS
  JsonRW.h
  MainWindow.h
  Track.h
  TrackControls.h
  Volume.h
)

SET(SOURCES
  JsonRW.cpp
  Main.cpp
  MainWindow.cpp
  Track.cpp
  TrackControls.cpp
  Volume.cpp
)

SET(RESOURCES
  ${PROJECT_SOURCE_DIR}/icons/icons.qrc
  ${PROJECT_SOURCE_DIR}/styles/styles.qrc
)

ADD_LIBRARY(${APP_LIB} STATIC
  ${HEADERS}
  ${SOURCES}
)
TARGET_LINK_LIBRARIES(${APP_LIB}
  Qt::Widgets
)

ADD_EXECUTABLE(${APP_EXE}
  ${RESOURCES}
)

TARGET_LINK_LIBRARIES(${APP_EXE} PRIVATE
  ${APP_LIB}
)
