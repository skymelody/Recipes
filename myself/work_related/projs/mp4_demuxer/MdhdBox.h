//
// Created by skymelody on 2019/12/17.
//

#ifndef MP4TOH264_MDHDBOX_H
#define MP4TOH264_MDHDBOX_H

#include "Box.h"

class MdhdBox : public Box {
public:
    MdhdBox();
    void Init(uint64_t creation_time,
              uint64_t modification_time,
              uint32_t timescale,
              uint64_t duration,
              const char* language);

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
    uint32_t timescale_;
    union {
        uint32_t duration_;
        uint64_t duration_64_;
    };
    std::string language_;
};


#endif //MP4TOH264_MDHDBOX_H
