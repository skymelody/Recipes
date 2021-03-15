//
// Created by skymelody on 2019/12/17.
//

#include "TkhdBox.h"
#include <numeric>
#include <cstring>

using namespace std;
void
TkhdBox::Init(uint64_t creation_time,
              uint64_t modification_time,
              uint32_t track_id,
              uint64_t duration,
              int16_t volume,
              uint32_t width, uint32_t height) {
    version_ = modification_time > numeric_limits<uint32_t>::max() ? 1 : 0;
    flags_[0] = 0;
    flags_[1] = 0;
    flags_[2] = 7;

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
    track_id_ = track_id+1;
    layer_ = 0;
    alternate_group_ = 0;
    volume_ = volume;

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

    width_ = width;
    height_ = height;
    size += 68;
}

TkhdBox::TkhdBox() {
    size = 8;
    Byte tp[4]{'t', 'k', 'h', 'd'};
    memmove(&type, tp, 4);
}

void TkhdBox::WriteAttr(FILE *out_file) {
    fwrite(&version_, 1, 1, out_file);
    fwrite(flags_.data(), 1, 3, out_file);
    Byte reserved[64]{0};
    if (version_ == 0) {
            uint32_t bcreatetime = LittleEndToBigEnd(creation_time_);
            uint32_t bmodifytime = LittleEndToBigEnd(modification_time_);
            uint32_t btrackid = LittleEndToBigEnd(track_id_);
            uint32_t bduration = LittleEndToBigEnd(duration_);
            fwrite(&bcreatetime, 1, 4, out_file);
            fwrite(&bmodifytime, 1, 4, out_file);
            fwrite(&btrackid, 1, 4, out_file);
            fwrite(reserved, 1, 4, out_file);
            fwrite(&bduration, 1, 4, out_file);
    }
    else {

    }
    fwrite(reserved, 1, 8, out_file);
    int16_t blayer = LittleEndToBigEnd(layer_);
    fwrite(&blayer, 1, 2, out_file);
    int16_t balgroup = LittleEndToBigEnd(alternate_group_);
    fwrite(&balgroup, 1, 2, out_file);
    int16_t bvolume = LittleEndToBigEnd(volume_);
    fwrite(&bvolume, 1, 2, out_file);
    fwrite(reserved, 1, 2, out_file);
    for (auto entry : matrix_) {
        auto tmp = LittleEndToBigEnd(entry);
        fwrite(&tmp, 1, 4, out_file);
    }
    uint32_t bwidth = LittleEndToBigEnd(width_);
    uint32_t bheight = LittleEndToBigEnd(height_);
    fwrite(&bwidth, 1, 4, out_file);
    fwrite(&bheight, 1, 4, out_file);

}
