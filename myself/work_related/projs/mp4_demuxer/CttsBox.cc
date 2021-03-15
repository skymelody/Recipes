//
// Created by skymelody on 2019/12/17.
//

#include <cstring>
#include "CttsBox.h"

void CttsBox::AddEntry(uint32_t count, __uint32_t ctsOffset) {
    count_offset_.emplace_back(count, ctsOffset);
    size += 8;
}

CttsBox::CttsBox() {
    Byte tp[4]{'c', 't', 't', 's'};
    memmove(&type, tp, 4);
    version_ = 0;
    memset(flags_.data(), 0, flags_.size());
    size = 16;
}

void CttsBox::WriteAttr(FILE *out_file) {
    fwrite(&version_, 1, 1, out_file);
    fwrite(flags_.data(), 1, 3, out_file);
    uint32_t bcount = LittleEndToBigEnd((uint32_t)count_offset_.size());
    fwrite(&bcount, 1, 4, out_file);
    for (int i = 0; i < count_offset_.size(); ++i) {
        uint32_t tmp = LittleEndToBigEnd(count_offset_[i].first);
        fwrite(&tmp, 1, 4, out_file);
        tmp = LittleEndToBigEnd(count_offset_[i].second);
        fwrite(&tmp, 1, 4, out_file);
    }
}
