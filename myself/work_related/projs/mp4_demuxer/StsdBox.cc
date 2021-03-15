//
// Created by skymelody on 2019/12/10.
//

#include "StsdBox.h"
#include "ByteConvert.h"
#include <cstring>
#include <iostream>

size_t StsdBox::ParseAttr(FileStreamReader &reader) {
    int nread = 0;
    int attrSize = 0;
    Byte buf[16];

    /* version */
    nread = reader.ReadNByte(buf, 1);
    memmove(&version, buf, nread);
    attrSize += nread;

    /* flags */
    nread = reader.ReadNByte(buf, 3);
    memmove(flags.data(), buf, nread);
    attrSize += nread;

    /* entry count */
    nread = reader.ReadNByte(buf, 4);
    memmove(&entryCount, buf, nread);
    attrSize += nread;

    return attrSize;
}



StsdBox::StsdBox() {
    size = 8;
    Byte tp[4]{'s', 't', 's', 'd'};
    memmove(&type, tp, 4);
}

void StsdBox::Init(int entryCount, Box* sampleEntry) {
    version = 0;
    memset(flags.data(), 0, flags.size());
    this->entryCount = entryCount;
    this->sampleEntry = sampleEntry;
    size += 8;
    size += sampleEntry->Size();
}

void StsdBox::WriteAttr(FILE *out_file) {
    fwrite(&version, 1, 1, out_file);
    fwrite(flags.data(), 1, 3, out_file);
    uint32_t bcount = LittleEndToBigEnd(entryCount);
    fwrite(&bcount, 1, 4, out_file);
    sampleEntry->Write(out_file);
}
