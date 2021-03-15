//
// Created by skymelody on 2019/12/17.
//

#ifndef MP4TOH264_CTTSBOX_H
#define MP4TOH264_CTTSBOX_H

#include "Box.h"

class CttsBox : public Box {
public:
    CttsBox();
    void AddEntry(uint32_t count, uint32_t ctsOffset);

protected:
    void WriteAttr(FILE *out_file) override;

private:
    Byte version_;
    std::array<Byte, 3> flags_;
    uint32_t entry_count_;
    std::vector<std::pair<uint32_t, uint32_t>> count_offset_;
};


#endif //MP4TOH264_CTTSBOX_H
