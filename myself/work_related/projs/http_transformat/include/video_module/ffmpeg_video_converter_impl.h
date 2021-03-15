#ifndef FFMPEG_VIDEO_CONVERTER_IMPL_INCLUDED
#define FFMPEG_VIDEO_CONVERTER_IMPL_INCLUDED

#include "video.h"


class FFMpegVideoConverterImpl {
public:
    bool Convert(Video& inputVideo, Video& outputVideo);
};

#endif