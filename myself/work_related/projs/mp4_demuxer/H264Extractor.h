//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_H264EXTRACTOR_H
#define MP4TOH264_H264EXTRACTOR_H

#include <string>
#include "Mp4Parser.h"

class H264Extractor {
public:
    bool Extract(const std::string& inputFilePath, const std::string& outputFilePath);

private:
    bool DoExtract(Mp4Parser& parser, const std::string& outputPath);
};


#endif //MP4TOH264_H264EXTRACTOR_H
