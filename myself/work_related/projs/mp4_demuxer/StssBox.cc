//
// Created by skymelody on 2019/12/17.
//

#include "StssBox.h"
#include <cstring>

void StssBox::AddEntry(uint32_t sample) {
    sample_number_.push_back(sample);
    size += 4;
}

uint32_t StssBox::EntryCount() const {
    return sample_number_.size();
}

StssBox::StssBox() {
    size = 8;
    Byte tp[4]{'s', 't', 's', 's'};
    version_ = 0;
    memset(flags_.data(), 0, flags_.size());
    memmove(&type, tp, 4);

    size += 8;
}

void StssBox::WriteAttr(FILE *out_file) {
    fwrite(&version_, 1, 1, out_file);
    fwrite(flags_.data(), 1, 3, out_file);
    uint32_t bcount = LittleEndToBigEnd((uint32_t)sample_number_.size());
    fwrite(&bcount, 1, 4, out_file);
    for (int i = 0; i < sample_number_.size(); ++i) {
        uint32_t tmp = LittleEndToBigEnd(sample_number_[i]);
        fwrite(&tmp, 1, 4, out_file);
    }
}
