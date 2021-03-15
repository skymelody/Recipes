//
// Created by skymelody on 2019/12/18.
//

#include "DrefBox.h"
#include <cstring>

void DrefBox::Init(Box *entry_box, int entry_count) {
    version_ = 0;
    memset(flags_.data(), 0, flags_.size());
    entry_count_ = entry_count;

    size += 8;
    entry_box_ = entry_box;
    for (int i =0 ; i < entry_count; ++i) {
        size += entry_box->Size();
    }
}

DrefBox::~DrefBox() {
    delete entry_box_;
}

DrefBox::DrefBox() {
    size = 8;
    Byte tp[4]{'d', 'r', 'e', 'f'};
    memmove(&type, tp, 4);
}

void DrefBox::WriteAttr(FILE *out_file) {
    fwrite(&version_, 1, 1, out_file);
    fwrite(flags_.data(), 1, 3, out_file);
    uint32_t bcount = LittleEndToBigEnd(entry_count_);
    fwrite(&bcount, 1, 4, out_file);
    entry_box_->Write(out_file);
}
