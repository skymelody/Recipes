//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_AVC1BOX_H
#define MP4TOH264_AVC1BOX_H

#include "Box.h"

class Avc1Box : public Box {
public:
    Avc1Box();
    void Init(uint32_t width, uint32_t height);
protected:
    void WriteAttr(FILE *out_file) override;

    size_t ParseAttr(FileStreamReader &reader) override;


private:
    uint16_t dataRefIndex;
    uint16_t width;
    uint16_t height;
    uint32_t horResolution;
    uint32_t verResolution;
    uint16_t frameCount;
    std::string compressorName;
    uint16_t depth;
};


#endif //MP4TOH264_AVC1BOX_H
