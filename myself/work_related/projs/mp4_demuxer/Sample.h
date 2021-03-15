//
// Created by skymelody on 2019/12/13.
//

#ifndef MP4TOH264_SAMPLE_H
#define MP4TOH264_SAMPLE_H

#include <vector>
#include <cstdint>

using Byte = unsigned char;


class Sample {
public:
    Sample();

    void Clear();
    std::vector<Byte> data;
    bool isIdr;
    uint32_t offset;
    uint32_t duration;
    uint32_t descriptionIndex;
    uint32_t dts;
    uint32_t ctsDelta;

    uint32_t decodeOrder;
    uint32_t displayOrder;
    int nalCnt;
};


#endif //MP4TOH264_SAMPLE_H
