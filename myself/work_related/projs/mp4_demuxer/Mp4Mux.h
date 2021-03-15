//
// Created by skymelody on 2019/12/12.
//

#ifndef MP4TOH264_MP4MUX_H
#define MP4TOH264_MP4MUX_H

#include <string>

class Mp4Mux {
public:
    bool Mux(const std::string& h264Path, const std::string& outputPath);
};


#endif //MP4TOH264_MP4MUX_H
