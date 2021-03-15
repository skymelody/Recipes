//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_STSZBOX_H
#define MP4TOH264_STSZBOX_H

#include "Box.h"
#include "Sample.h"

class StszBox : public Box {
public:
    StszBox();
    std::vector<uint32_t> EntrySizeVec() const;
    void Init(const std::vector<uint32_t>& samples);
    void AddEntry(uint32_t size);

protected:
    size_t ParseAttr(FileStreamReader &reader) override;

    void WriteAttr(FILE *out_file) override;

private:
    uint8_t version;
    std::array<uint8_t, 3> flags;
    uint32_t sampleSize;
    uint32_t sampleCount;
    std::vector<uint32_t> entrySizeVec;
};


#endif //MP4TOH264_STSZBOX_H
