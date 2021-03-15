//
// Created by skymelody on 2019/12/13.
//

#ifndef MP4TOH264_NALUPARSER_H
#define MP4TOH264_NALUPARSER_H

#include <vector>
#include "FileStreamReader.h"

using Byte = unsigned char;

enum NaluType {
    NALU_TYPE_SLICE    = 1,
    NALU_TYPE_DPA      = 2,
    NALU_TYPE_DPB      = 3,
    NALU_TYPE_DPC      = 4,
    NALU_TYPE_IDR      = 5,
    NALU_TYPE_SEI      = 6,
    NALU_TYPE_SPS      = 7,
    NALU_TYPE_PPS      = 8,
    NALU_TYPE_AUD      = 9,
    NALU_TYPE_EOSEQ    = 10,
    NALU_TYPE_EOSTREAM = 11,
    NALU_TYPE_FILL     = 12,
};

struct Nalu {
    int prefixLen;
    std::vector<Byte> data;
};

class NaluParser {
public:
    NaluParser();
    // false if failed
    bool GetOneNalu(Nalu& nalu, FileStreamReader& reader);

private:
    void LoadBuffer(FileStreamReader& reader);
private:

    const static int BUFFER_SIZE;
    const static int LOW_BOUND_LOAD_BUFFER;
    std::vector<Byte> buffer;
    int cur;
};


#endif //MP4TOH264_NALUPARSER_H
