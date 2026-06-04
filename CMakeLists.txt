cmake_minimum_required(VERSION 3.15)

project(TeXtractor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

add_compile_definitions(TEXTRACTOR_VERSION="${PROJECT_VERSION}")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(TeXtractor ${SOURCES})
set_target_properties(TeXtractor PROPERTIES OUTPUT_NAME textractor)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
pkg_check_modules(LEPT REQUIRED lept)
find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

target_include_directories(TeXtractor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPT_INCLUDE_DIRS}
)

target_link_libraries(TeXtractor PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
    CURL::libcurl
    ${TESSERACT_LIBRARIES}
    ${LEPT_LIBRARIES}
)

# Install the application and its resources
include(GNUInstallDirs)
install(TARGETS TeXtractor DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES textractor.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES textractor.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps)
