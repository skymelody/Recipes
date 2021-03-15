//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_AVCCBOX_H
#define MP4TOH264_AVCCBOX_H

#include "Box.h"


struct Sps {
    uint16_t len;
    std::vector<uint8_t> sps;
};

struct Pps {
    uint16_t len;
    std::vector<uint8_t> pps;
};

class AvcCBox : public Box {
public:
    AvcCBox();
    std::vector<Sps> GetSps() const;
    std::vector<Pps> GetPps() const;
    int PrefixLen() const;
    void Init(uint8_t profile,
              uint8_t compatibility,
              uint8_t level,
              uint8_t lenSize,
              const std::vector<Sps>& sps,
              const std::vector<Pps>& pps);

protected:
    size_t ParseAttr(FileStreamReader &reader) override;

    void WriteAttr(FILE *out_file) override;

private:
    uint8_t version;
    uint8_t profile;
    uint8_t compatibility;
    uint8_t level;
    uint8_t lenSizeMinusOne;
    uint8_t spsNum;
    std::vector<Sps> sps;
    uint8_t ppsNum;
    std::vector<Pps> pps;
};


#endif //MP4TOH264_AVCCBOX_H
