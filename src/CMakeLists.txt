SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(Qt6 COMPONENTS Widgets REQUIRED)

SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOMOC ON)

SET(HEADERS
  JsonRW.h
  MainWindow.h
  Track.h
  TrackControls.h
)

SET(SOURCES
  JsonRW.cpp
  Main.cpp
  MainWindow.cpp
  Track.cpp
  TrackControls.cpp
)

SET(RESOURCES
  ${PROJECT_SOURCE_DIR}/icons/icons.qrc
  ${PROJECT_SOURCE_DIR}/styles/styles.qrc
)

ADD_EXECUTABLE(${PROJECT_NAME}
  ${HEADERS}
  ${SOURCES}
  ${RESOURCES}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE Qt::Widgets)