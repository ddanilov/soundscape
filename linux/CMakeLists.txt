install(DIRECTORY "applications" TYPE DATA)
install(DIRECTORY "icons" TYPE DATA)

string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
configure_file(metainfo/soundscape.metainfo.xml.in metainfo/soundscape.metainfo.xml)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/metainfo" TYPE DATA)
