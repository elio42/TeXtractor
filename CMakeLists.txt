cmake_minimum_required(VERSION 3.16)

project(TeXtractor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
pkg_check_modules(LEPT REQUIRED lept)
find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

set(TEXTRACTOR_SOURCES
    src/core/main.cpp
    src/gui/main_window.cpp
    src/gui/main_pages/home_page.cpp
    src/gui/main_pages/result_page.cpp
    src/gui/main_pages/settings_page.cpp
    src/core/settings.cpp
    src/core/open_terminal.cpp
    src/ocr/ocr.cpp
    src/ai-recognition/ai_client.cpp
    src/ai-recognition/gemini_client.cpp
    src/ai-recognition/ollama_client.cpp
    src/include/base64.cpp
)

add_executable(TeXtractor ${TEXTRACTOR_SOURCES})

target_include_directories(TeXtractor PRIVATE
    src
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPT_INCLUDE_DIRS}
)

target_compile_options(TeXtractor PRIVATE
    ${TESSERACT_CFLAGS_OTHER}
    ${LEPT_CFLAGS_OTHER}
)

target_link_libraries(TeXtractor PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
    CURL::libcurl
    ${TESSERACT_LIBRARIES}
    ${LEPT_LIBRARIES}
)
