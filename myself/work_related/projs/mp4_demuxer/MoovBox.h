//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_MOOVBOX_H
#define MP4TOH264_MOOVBOX_H
#include "Box.h"
#include <cstring>

class MoovBox : public Box {
public:

public:
    MoovBox() {
        size = 8;
        Byte tp[4]{'m', 'o', 'o', 'v'};
        memmove(&type, tp, 4);
    }
};



#endif //MP4TOH264_MOOVBOX_H
