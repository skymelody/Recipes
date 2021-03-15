//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_STCOBOX_H
#define MP4TOH264_STCOBOX_H

#include "Box.h"

class StcoBox : public Box {
public:
    StcoBox();
    std::vector<uint32_t> ChunkOffset() const;
    void AddEntry(uint32_t offset);

    void AdjustChunkOffset(uint32_t mdatoffset);

protected:
    size_t ParseAttr(FileStreamReader &reader) override;

    void WriteAttr(FILE *out_file) override;

private:
    uint8_t version;
    std::array<uint8_t, 3> flags;

    uint32_t entryCount;
    std::vector<uint32_t> chunkOffset;
};


#endif //MP4TOH264_STCOBOX_H
