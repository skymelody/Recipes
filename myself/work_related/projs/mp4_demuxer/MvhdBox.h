//
// Created by skymelody on 2019/12/18.
//

#ifndef MP4TOH264_MVHDBOX_H
#define MP4TOH264_MVHDBOX_H

#include "Box.h"

class MvhdBox : public Box {
public:
    MvhdBox();
    void Init(uint64_t creation_time,
              uint64_t modification_time,
              uint32_t timescale,
              uint64_t duration);

protected:
    void WriteAttr(FILE *out_file) override;

private:
    Byte version_;
    std::array<Byte, 3> flags;
    union {
        uint32_t creation_time_;
        uint64_t creation_time_64_;
    };
    union {
        uint32_t modification_time_;
        uint64_t modification_time_64_;
    };
    uint32_t timescale_;
    union {
        uint32_t duration_;
        uint64_t duration_64_;
    };
    int32_t rate_;
    int16_t volume_;
    std::array<int32_t, 9> matrix_;
    uint32_t next_track_id_;

};


#endif //MP4TOH264_MVHDBOX_H
