//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_TRAKBOX_H
#define MP4TOH264_TRAKBOX_H
#include "Box.h"


class TrakBox : public Box {
public:
    TrakBox() {
        size = 8;
        Byte tp[4]{'t', 'r', 'a', 'k'};
        memmove(&type, tp, 4);
    }

};



#endif //MP4TOH264_TRAKBOX_H
