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
  TransitionIcon.cpp
  Volume.cpp
)

set(RESOURCES
  ${PROJECT_SOURCE_DIR}/icons/icons.qrc
  ${PROJECT_SOURCE_DIR}/icons/win32.rc
  ${PROJECT_SOURCE_DIR}/styles/styles.qrc
)

add_library(${APP_LIB} STATIC
  ${HEADERS}
  ${SOURCES}
)
target_link_libraries(${APP_LIB} Qt::Multimedia Qt::Widgets)

add_executable(${APP_EXE} ${RESOURCES})
target_link_libraries(${APP_EXE} PRIVATE ${APP_LIB})

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

    install(FILES ${PROJECT_SOURCE_DIR}/README.md DESTINATION ${APP_TARGET_DIR})
    install(FILES ${PROJECT_SOURCE_DIR}/LICENSE DESTINATION ${APP_TARGET_DIR})
    install(TARGETS ${APP_EXE} DESTINATION ${APP_TARGET_DIR})
    install(CODE "execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --compiler-runtime \"${APP_TARGET_DIR}\")")
endif(WIN32)
