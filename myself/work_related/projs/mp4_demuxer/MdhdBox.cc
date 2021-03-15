//
// Created by skymelody on 2019/12/17.
//

#include "MdhdBox.h"
#include <numeric>
#include <cstring>

using namespace std;
void MdhdBox::Init(uint64_t creation_time,
                   uint64_t modification_time,
                   uint32_t timescale,
                   uint64_t duration,
                   const char* language) {
    version_ = modification_time > numeric_limits<uint32_t>::max() ? 1 : 0;
    flags_[0] = flags_[1] = flags_[2] = 0;

    size += 4;

    if (version_ == 0) {
        creation_time_ = creation_time;
        modification_time_ = modification_time;
        duration_ = duration;
        size += 12;
    }
    else {
        creation_time_64_ = creation_time;
        modification_time_64_ = modification_time;
        duration_64_ = duration;
        size += 24;
    }
    timescale_ = timescale;
    language_ = language;
    size += 8;
}

MdhdBox::MdhdBox() {
    size = 8;
    Byte tp[4]{'m', 'd', 'h', 'd'};
    memmove(&type, tp, 4);
}

void MdhdBox::WriteAttr(FILE *out_file) {
    fwrite(&version_, 1, 1, out_file);
    fwrite(flags_.data(), 1, 3, out_file);
    if (version_ == 0) {
        uint32_t bcreatetime = LittleEndToBigEnd(creation_time_);
        uint32_t bmodifytime = LittleEndToBigEnd(modification_time_);
        uint32_t btimescale = LittleEndToBigEnd(timescale_);
        uint32_t bduration = LittleEndToBigEnd(duration_);
        fwrite(&bcreatetime, 1, 4, out_file);
        fwrite(&bmodifytime, 1, 4, out_file);
        fwrite(&btimescale, 1, 4, out_file);
        fwrite(&bduration, 1, 4, out_file);
    }
    else {

    }

    Byte predefind[2]{0x55, 0xc4};
    fwrite(predefind, 1, 2, out_file);
    memset(predefind, 0, 2);
    fwrite(predefind, 1, 2, out_file);
}
