//
// Created by skymelody on 2019/12/18.
//

#include "MvhdBox.h"
#include <numeric>
#include <cstring>
using namespace std;

void MvhdBox::Init(uint64_t creation_time,
                   uint64_t modification_time,
                   uint32_t timescale,
                   uint64_t duration) {
    version_ = 0;
    memset(flags.data(), 0, flags.size());
    size += 4;
    if (creation_time > numeric_limits<uint32_t>::max()) {
        version_ = 1;
    }
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
    rate_ = 0x00010000;
    volume_ = 0x0100;

    {
        matrix_[0] = 0x00010000;
        matrix_[1] = 0;
        matrix_[2] = 0;
        matrix_[3] = 0;
        matrix_[4] = 0x00010000;
        matrix_[5] = 0;
        matrix_[6] = 0;
        matrix_[7] = 0;
        matrix_[8] = 0x40000000;
    }

    next_track_id_ = 0xffffffff;
    size += 84;
}

MvhdBox::MvhdBox() {
    size = 8;
    Byte tp[4]{'m', 'v', 'h', 'd'};
    memmove(&type, tp, 4);
}

void MvhdBox::WriteAttr(FILE *out_file) {
    int nwrite = 0;
    nwrite += fwrite(&version_, 1, 1, out_file);
    nwrite += fwrite(flags.data(), 1, 3, out_file);
    if (version_ == 0) {
        uint32_t bcreation_time = LittleEndToBigEnd(creation_time_);
        uint32_t bmodify_time = LittleEndToBigEnd(modification_time_);
        uint32_t btimescale = LittleEndToBigEnd(timescale_);
        uint32_t bduration = LittleEndToBigEnd(duration_);
        nwrite += fwrite(&bcreation_time, 1, 4, out_file);
        nwrite += fwrite(&bmodify_time, 1, 4, out_file);
        nwrite += fwrite(&btimescale, 1, 4, out_file);
        nwrite += fwrite(&bduration, 1, 4, out_file);
    }
    else {
        uint64_t bcreation_time = LittleEndToBigEnd(creation_time_64_);
        uint64_t bmodify_time = LittleEndToBigEnd(modification_time_64_);
        uint32_t btimescale = LittleEndToBigEnd(timescale_);
        uint64_t bduration = LittleEndToBigEnd(duration_64_);
        fwrite(&bcreation_time, 1, 8, out_file);
        fwrite(&bmodify_time, 1, 8, out_file);
        fwrite(&btimescale, 1, 4, out_file);
        fwrite(&bduration, 1, 8, out_file);
    }
    int32_t brate = LittleEndToBigEnd(rate_);
    nwrite += fwrite(&brate, 1, 4, out_file);
    int16_t bvolume = htons(volume_);
    nwrite += fwrite(&bvolume, 1, 2, out_file);
    Byte reserved[10]{0};
    nwrite += fwrite(reserved, 1, 10, out_file);
    for (int32_t entry : matrix_) {
        int32_t bentry = LittleEndToBigEnd(entry);
        nwrite += fwrite(&bentry, 1, 4, out_file);
    }
    Byte predefined[24]{0};
    nwrite += fwrite(predefined, 1, 24, out_file);
    uint32_t btrakId = LittleEndToBigEnd(next_track_id_);
    nwrite += fwrite(&btrakId, 1, 4, out_file);

}
