//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_MDIABOX_H
#define MP4TOH264_MDIABOX_H

#include "Box.h"
class MdiaBox : public Box {
public:
    MdiaBox() {
        size = 8;
        Byte tp[4]{'m', 'd', 'i', 'a'};
        memmove(&type, tp, 4);
    }
};
#endif //MP4TOH264_MDIABOX_H
