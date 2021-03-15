//
// Created by skymelody on 2019/12/18.
//

#ifndef MP4TOH264_DREFBOX_H
#define MP4TOH264_DREFBOX_H

#include "Box.h"

class DrefBox : public Box {
public:
    DrefBox();
    void Init(Box* entry_box, int entry_count);
    ~DrefBox() override;

protected:
    void WriteAttr(FILE *out_file) override;

private:
    Byte version_;
    std::array<Byte, 3> flags_;
    uint32_t entry_count_;
    Box* entry_box_;
};


#endif //MP4TOH264_DREFBOX_H
