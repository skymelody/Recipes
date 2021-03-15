#include "video_module/ffmpeg_video_converter.h"


FFMpegVideoConverter::~FFMpegVideoConverter() {}
bool FFMpegVideoConverter::Convert(Video& inputVideo, Video& outputVideo) {
    return _impl->Convert(inputVideo, outputVideo);
}