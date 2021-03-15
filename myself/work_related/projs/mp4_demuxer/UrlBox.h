//
// Created by skymelody on 2019/12/18.
//

#ifndef MP4TOH264_URLBOX_H
#define MP4TOH264_URLBOX_H

#include "Box.h"

class UrlBox : public Box {
public:
    UrlBox();

protected:
    void WriteAttr(FILE *out_file) override;

private:
    Byte version_;
    std::array<Byte, 3> flags_;
    std::string location_;

};


#endif //MP4TOH264_URLBOX_H
