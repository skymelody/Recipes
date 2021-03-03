cmake_minimum_required(VERSION 2.8)
project(QtVlc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${PROJECT_NAME} main.cc)

set(SYS_INC_PATH "/usr/lib/x86_64-linux-gnu")
set(SYS_LIB_PATH "/usr/lib/x86_64-linux-gnu")

# qt
find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(
  ${PROJECT_NAME} 
  Qt5::Widgets
  avcodec
  avformat
  avutil
  avfilter
  swscale
  vlc
  vlccore
)
