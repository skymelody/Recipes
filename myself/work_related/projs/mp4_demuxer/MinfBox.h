//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_MINFBOX_H
#define MP4TOH264_MINFBOX_H
#include "Box.h"

class MinfBox : public Box {
public:
    MinfBox() {
        size = 8;
        Byte tp[4]{'m', 'i', 'n', 'f'};
        memmove(&type, tp, 4);
    }

};


#endif //MP4TOH264_MINFBOX_H
