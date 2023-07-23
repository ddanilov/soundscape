# SPDX-FileCopyrightText: 2022-2023 Denis Danilov
# SPDX-License-Identifier: GPL-3.0-only

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
  #
  Version.h.in
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

message(STATUS "system name: ${CMAKE_SYSTEM_NAME}")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(ICON_NAME "icon")
  set(ICONSET_DIR "${ICON_NAME}.iconset")
  set(ICON_TMP "macos_icon.svg")
  # brew install librsvg
  find_program(RSVG_CONVERT rsvg-convert HINTS /usr/local/bin/ REQUIRED)
  add_custom_command(
    OUTPUT icon.icns
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ICONSET_DIR}
    COMMAND cat ${PROJECT_SOURCE_DIR}/icons/icon.svg | sed -e 's/r="16"/r="12"/' > ${ICON_TMP}
    COMMAND ${RSVG_CONVERT} ${ICON_TMP} --width=16   --height=16   --output=${ICONSET_DIR}/icon_16x16.png
    COMMAND ${RSVG_CONVERT} ${ICON_TMP} --width=32   --height=32   --output=${ICONSET_DIR}/icon_32x32.png
    COMMAND ${RSVG_CONVERT} ${ICON_TMP} --width=64   --height=64   --output=${ICONSET_DIR}/icon_64x64.png
    COMMAND ${RSVG_CONVERT} ${ICON_TMP} --width=128  --height=128  --output=${ICONSET_DIR}/icon_128x128.png
    COMMAND ${RSVG_CONVERT} ${ICON_TMP} --width=256  --height=256  --output=${ICONSET_DIR}/icon_256x256.png
    COMMAND ${RSVG_CONVERT} ${ICON_TMP} --width=512  --height=512  --output=${ICONSET_DIR}/icon_512x512.png
    COMMAND ${RSVG_CONVERT} ${ICON_TMP} --width=1024 --height=1024 --output=${ICONSET_DIR}/icon_1024x1024.png
    COMMAND iconutil -c icns ${ICONSET_DIR}
    DEPENDS ${PROJECT_SOURCE_DIR}/icons/icon.svg
  )

  set(APP_ICON "${ICON_NAME}.icns")
  set(MACOSX_BUNDLE_ICON_FILE ${APP_ICON})
  set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

qt_add_executable(${APP_EXE} WIN32 MACOSX_BUNDLE ${RESOURCES} ${APP_ICON})
target_link_libraries(${APP_EXE} PRIVATE ${APP_LIB})

install(TARGETS ${APP_EXE} RUNTIME BUNDLE DESTINATION ".")

set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATADIR}/${APP_NAME}")

if(WITH_DEPLOY_SCRIPT)
  if((CMAKE_SYSTEM_NAME MATCHES "Windows" AND Qt6_VERSION VERSION_GREATER_EQUAL 6.4)
    OR (CMAKE_SYSTEM_NAME MATCHES "Darwin" AND Qt6_VERSION VERSION_GREATER_EQUAL 6.4)
    OR (CMAKE_SYSTEM_NAME MATCHES "Linux" AND Qt6_VERSION VERSION_GREATER_EQUAL 6.5))

    set(CMAKE_INSTALL_DATADIR ".")
    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
      set(CMAKE_INSTALL_DATADIR "${APP_NAME}.app/Contents/Resources")
    endif()
    set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATADIR})

    qt_generate_deploy_app_script(TARGET ${APP_EXE} FILENAME_VARIABLE deploy_script)
    install(SCRIPT ${deploy_script})

    include(InstallRequiredSystemLibraries)

    set(CPACK_PACKAGE_VERSION_MAJOR "${APP_VERSION}")
    set(CPACK_PACKAGE_VERSION_MINOR "")
    set(CPACK_PACKAGE_VERSION_PATCH "")

    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
      set(CPACK_GENERATOR "ZIP")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
      set(CPACK_GENERATOR "DragNDrop")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
      set(CPACK_GENERATOR "TGZ")
    endif()

    include(CPack)

  endif()
endif()

message(STATUS "data dir: ${CMAKE_INSTALL_DATADIR}")
configure_file(Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
target_include_directories(${APP_LIB} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/examples" TYPE DATA)
install(FILES
  "${PROJECT_SOURCE_DIR}/README.md"
  "${PROJECT_SOURCE_DIR}/LICENSE"
  TYPE DOC)
