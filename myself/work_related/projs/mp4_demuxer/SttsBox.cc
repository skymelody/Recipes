//
// Created by skymelody on 2019/12/17.
//

#include "SttsBox.h"
#include <cstring>

void SttsBox::AddEntry(uint32_t sampleCount, uint32_t sampleDuration) {
    count_delta_.emplace_back(sampleCount, sampleDuration);
    size += 8;
}

uint32_t SttsBox::EntryCount() const {
    return count_delta_.size();

}


SttsBox::SttsBox() {
    size = 8;
    Byte tp[4]{'s', 't', 't', 's'};
    memmove(&type, tp, 4);
    version_ = 0;
    memset(flags_.data(), 0, flags_.size());

    size += 8;
}

void SttsBox::WriteAttr(FILE *out_file) {
    fwrite(&version_, 1, 1, out_file);
    fwrite(flags_.data(), 1, 3, out_file);
    int n = count_delta_.size();
    uint32_t bcount = LittleEndToBigEnd((uint32_t)count_delta_.size());
    fwrite(&bcount, 1, 4, out_file);
    for (int i = 0; i < count_delta_.size(); ++i) {
        uint32_t tmp = LittleEndToBigEnd(count_delta_[i].first);
        fwrite(&tmp, 1, 4, out_file);
        tmp = LittleEndToBigEnd(count_delta_[i].second);
        fwrite(&tmp, 1, 4, out_file);

    }
}
