//
// Created by skymelody on 2019/12/18.
//

#ifndef MP4TOH264_VMHDBOX_H
#define MP4TOH264_VMHDBOX_H

#include "Box.h"
#include <cstring>

class VmhdBox : public Box {
public:
    VmhdBox() {
        size = 20;
        Byte tp[4]{'v', 'm', 'h', 'd'};
        memmove(&type, tp, 4);
        version_ = 0;
        flags_[0] = flags_[1] = 0;
        flags_[2] = 1;
    }

protected:
    void WriteAttr(FILE *out_file) override {
        fwrite(&version_, 1, 1 ,out_file);
        fwrite(flags_.data(), 1, 3, out_file);
        Byte bytes[20]{0};
        fwrite(bytes, 1, 8, out_file);

    }

private:
    Byte version_;
    std::array<Byte, 3> flags_;
};


#endif //MP4TOH264_VMHDBOX_H
