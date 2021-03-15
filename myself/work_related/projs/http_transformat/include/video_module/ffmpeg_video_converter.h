#ifndef FFMPEG_VIDEO_CONVERTER_INCLUDED
#define FFMPEG_VIDEO_CONVERTER_INCLUDED

#include "video_converter.h"
#include "ffmpeg_video_converter_impl.h"

class FFMpegVideoConverter : public VideoConverter{
public:
    bool Convert(Video& inputVideo, Video& outputVideo);
    virtual ~FFMpegVideoConverter();
private:
    FFMpegVideoConverterImpl* _impl;
};


#endif