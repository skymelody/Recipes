include_directories(../include/)
aux_source_directory(./http_module HTTP_MODULE_SRCS)
aux_source_directory(./video_module VIDEO_MODULE_SRC)
aux_source_directory(./config_module CONFIG_MODULE_SRC)


add_executable(main main.cc ${HTTP_MODULE_SRCS} ${VIDEO_MODULE_SRC} ${CONFIG_MODULE_SRC})
target_link_libraries(main PocoNet PocoData PocoFoundation PocoJSON PocoUtil avformat avcodec avutil swresample swscale)
