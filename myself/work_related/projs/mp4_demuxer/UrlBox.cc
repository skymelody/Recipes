//
// Created by skymelody on 2019/12/18.
//

#include "UrlBox.h"
#include <cstring>

UrlBox::UrlBox() {
    size = 12;
    Byte tp[4]{'u', 'r', 'l', ' '};
    version_ = 0;
    flags_[0] = 0;
    flags_[1] = 0;
    flags_[2] = 1;
    memmove(&type, tp, 4);
}

void UrlBox::WriteAttr(FILE *out_file) {
    fwrite(&version_, 1, 1, out_file);
    fwrite(flags_.data(), 1, 3, out_file);
    fwrite(location_.c_str(), 1, location_.size(), out_file);
}
