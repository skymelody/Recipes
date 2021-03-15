//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_STSDBOX_H
#define MP4TOH264_STSDBOX_H

#include "Box.h"

struct SampleEntry {
    uint16_t dataRefIndex;
};

class StsdBox : public Box {
public:
    StsdBox();
    void Init(int entryCount, Box* sampleEntry);

protected:
    size_t ParseAttr(FileStreamReader &reader) override;

    void WriteAttr(FILE *out_file) override;

private:
    Box* sampleEntry;
    uint8_t version;
    std::array<uint8_t, 3> flags;
    uint32_t entryCount;
    std::vector<SampleEntry> sampleEntrys;
};


#endif //MP4TOH264_STSDBOX_H
