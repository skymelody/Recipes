//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_STBLBOX_H
#define MP4TOH264_STBLBOX_H
#include "Box.h"

class StblBox : public Box {
public:
    StblBox() {
        size = 8;
        Byte tp[4]{'s', 't', 'b', 'l'};
        memmove(&type, tp, 4);
    }
};

#endif //MP4TOH264_STBLBOX_H
