//
// Created by skymelody on 2019/12/9.
//

#ifndef MP4TOH264_HDLRBOX_H
#define MP4TOH264_HDLRBOX_H

#include "Box.h"

class HdlrBox : public Box {
public:
    HdlrBox();
    void Init(uint32_t hdlrtype, const std::string& name);
protected:
    size_t ParseAttr(FileStreamReader &reader) override;

    void WriteAttr(FILE *out_file) override;

private:
    uint8_t version;
    std::array<uint8_t, 3> flags;
    uint32_t handlerType;
    std::string name;
};


#endif //MP4TOH264_HDLRBOX_H
