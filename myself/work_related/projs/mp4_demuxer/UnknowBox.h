//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_UNKNOWBOX_H
#define MP4TOH264_UNKNOWBOX_H

#include "Box.h"

class UnknowBox : public Box {
protected:
    size_t ParseAttr(FileStreamReader &reader) override;
};


#endif //MP4TOH264_UNKNOWBOX_H
