cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(MediaPlayer)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Programs/Qt/5.15.2/msvc2019_64/lib/cmake")

# Find the Qt5 package
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL Multimedia)

# Add the source files to your project
set(SOURCES
    audioplay.cpp
    audiothreadutil.cpp
    decodethreadutil.cpp
    decodeutil.cpp
    demuxthreadutil.cpp
    demuxutil.cpp
    main.cpp
    mainwindow.cpp
    progressbarslider.cpp
    resampleutil.cpp
    videoplaywidget.cpp
    videothreadutil.cpp
)

set(HEADERS
    audioplay.h
    audiothreadutil.h
    decodethreadutil.h
    decodeutil.h
    demuxthreadutil.h
    demuxutil.h
    mainwindow.h
    progressbarslider.h
    resampleutil.h
    videoplaywidget.h
    videothreadutil.h
)

# Add the FFmpeg library and include directories
include_directories(C:\\CommandLineTools\\ffmpeg-6.0-full_build-shared\\include)
link_directories(C:\\CommandLineTools\\ffmpeg-6.0-full_build-shared\\lib)

# Specify your target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link the Qt libraries and FFmpeg libraries to your target
target_link_libraries(${PROJECT_NAME}
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::OpenGL
    Qt::Multimedia
    avcodec
    avdevice
    avfilter
    avformat
    avutil
    postproc
    swresample
    swscale
)
