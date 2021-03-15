//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_STSCBOX_H
#define MP4TOH264_STSCBOX_H

#include "Box.h"
#include <vector>

struct ChunkSampleInfo {
    uint32_t firstChunk;
    uint32_t samplesPerChunk;
    uint32_t sampleDescriptionIndex;
};

class StscBox : public Box {
public:
    StscBox();
    uint32_t EntryCount() const;
    std::vector<ChunkSampleInfo> SampleInfos() const;
    void AddEntry(const ChunkSampleInfo& sampleInfo);

protected:
    size_t ParseAttr(FileStreamReader &reader) override;

    void WriteAttr(FILE *out_file) override;

private:
    uint8_t version;
    std::array<uint8_t, 3> flags;
    uint32_t entryCount;
    std::vector<ChunkSampleInfo> sampleInfos;
};


#endif //MP4TOH264_STSCBOX_H
