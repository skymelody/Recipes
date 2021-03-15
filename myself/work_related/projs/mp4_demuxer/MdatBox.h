//
// Created by skymelody on 2019/12/10.
//

#ifndef MP4TOH264_MDATBOX_H
#define MP4TOH264_MDATBOX_H

#include "Box.h"

class MdatBox : public Box {
public:
    MdatBox();
    FILE* GetFile() const;
    void SetFile(FILE* file);

protected:
    size_t ParseAttr(FileStreamReader &reader) override;

    void WriteAttr(FILE *out_file) override;

private:
    FILE* file;
};


#endif //MP4TOH264_MDATBOX_H
