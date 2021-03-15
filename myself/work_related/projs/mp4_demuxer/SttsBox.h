//
// Created by skymelody on 2019/12/17.
//

#ifndef MP4TOH264_STTSBOX_H
#define MP4TOH264_STTSBOX_H


#include "Box.h"

class SttsBox : public Box{
public:
    SttsBox();
    uint32_t EntryCount() const;
    void AddEntry(uint32_t sampleCount, uint32_t sampleDuration);

protected:
    void WriteAttr(FILE *out_file) override;

private:
    Byte version_;
    std::array<Byte, 3> flags_;
    uint32_t entry_count_;
    std::vector<std::pair<uint32_t, uint32_t>> count_delta_;
};


#endif //MP4TOH264_STTSBOX_H
