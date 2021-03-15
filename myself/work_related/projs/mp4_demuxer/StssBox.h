//
// Created by skymelody on 2019/12/17.
//

#ifndef MP4TOH264_STSSBOX_H
#define MP4TOH264_STSSBOX_H

#include "Box.h"

class StssBox : public Box {
public:
    StssBox();

    uint32_t EntryCount() const;
    void AddEntry(uint32_t sample);

protected:
    void WriteAttr(FILE *out_file) override;

private:
    Byte version_;
    std::array<Byte, 3> flags_;
    uint32_t entry_count_;
    std::vector<uint32_t> sample_number_;
};


#endif //MP4TOH264_STSSBOX_H
