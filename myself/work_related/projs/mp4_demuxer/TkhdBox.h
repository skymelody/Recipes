//
// Created by skymelody on 2019/12/17.
//

#ifndef MP4TOH264_TKHDBOX_H
#define MP4TOH264_TKHDBOX_H

#include "Box.h"

class TkhdBox : public Box {
public:
    TkhdBox();
    void Init(uint64_t creation_time,
              uint64_t modification_time,
              uint32_t track_id,
              uint64_t duration,
              int16_t volume,
              uint32_t width,
              uint32_t height);

protected:
    void WriteAttr(FILE *out_file) override;

private:
    Byte version_;
    std::array<Byte, 3> flags_;
    union {
        uint32_t creation_time_;
        uint64_t creation_time_64_;
    };
    union {
        uint32_t modification_time_;
        uint64_t modification_time_64_;
    };
    uint32_t track_id_;
    union  {
        uint32_t duration_;
        uint64_t duration_64_;
    };
    int16_t layer_;
    int16_t alternate_group_;
    int16_t volume_;
    std::array<int32_t, 9> matrix_;
    uint32_t width_;
    uint32_t height_;
};


#endif //MP4TOH264_TKHDBOX_H
