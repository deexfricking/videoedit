cmake_minimum_required(VERSION 3.16)
project(VideoEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 required components
find_package(Qt6 REQUIRED COMPONENTS
    Core Widgets Multimedia MultimediaWidgets Concurrent)

# Optional FFmpeg via pkg-config
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(FFMPEG QUIET libavcodec libavformat libavutil libswscale libswresample)
    if(FFMPEG_FOUND)
        message(STATUS "FFmpeg found — export/thumbnail features enabled.")
        add_compile_definitions(USE_FFMPEG)
    else()
        message(WARNING "FFmpeg not found — export will be limited. Install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev")
    endif()
endif()

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Timeline.cpp
    src/MediaBin.cpp
    src/VideoPlayer.cpp
    src/ToolBar.cpp
    src/CropDialog.cpp
    src/CanvasSettingsDialog.cpp
    src/ThumbnailGenerator.cpp
    src/ExportDialog.cpp
    src/ProjectManager.cpp
    src/EffectsPanel.cpp
)

set(HEADERS
    src/MainWindow.h
    src/Timeline.h
    src/TimelineClip.h
    src/MediaBin.h
    src/VideoPlayer.h
    src/ToolBar.h
    src/CropDialog.h
    src/CanvasSettingsDialog.h
    src/ThumbnailGenerator.h
    src/ExportDialog.h
    src/ProjectManager.h
    src/EffectsPanel.h
)

add_executable(VideoEditor ${SOURCES} ${HEADERS})

target_link_libraries(VideoEditor PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Concurrent
)

if(FFMPEG_FOUND)
    target_include_directories(VideoEditor PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(VideoEditor PRIVATE ${FFMPEG_LIBRARIES})
endif()

# Install target
install(TARGETS VideoEditor DESTINATION bin)
