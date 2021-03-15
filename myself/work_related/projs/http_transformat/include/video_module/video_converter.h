#ifndef VIDEO_CONVERTER_INCLUDED
#define VIDEO_CONVERTER_INCLUDED

#include "video.h"
class VideoConverter {
public:
    virtual bool Convert(Video& inputVideo, Video& outputVideo) = 0;
    virtual ~VideoConverter();
};

#endif