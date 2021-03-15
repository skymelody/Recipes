//
// Created by skymelody on 2019/12/9.
//

#include "HdlrBox.h"
#include "ByteConvert.h"
#include <cstring>
#include <iostream>
using namespace std;

size_t HdlrBox::ParseAttr(FileStreamReader &reader) {
    Byte buf[8];
    size_t attrSize = 0;

    int nread = 0;

    /* version */
    nread = reader.ReadNByte(buf, 1);
    memmove(&version, buf, 1);
    attrSize += nread;

    /* flags */
    nread = reader.ReadNByte(buf, 3);
    memmove(flags.data(), buf, 1);
    attrSize += nread;

    /* predefined */
    nread = reader.ReadNByte(buf, 4);
    attrSize += nread;

    /* handler type */
    nread = reader.ReadNByte(buf, 4);
    memmove(&handlerType, buf, nread);
    attrSize += nread;


    /* reserved */
    for (int i = 0; i < 3; ++i) {
        nread = reader.ReadNByte(buf, 4);
        attrSize += nread;

    }

    /* name */
    const size_t nameSize = BytesToInt(size) - headerSize - attrSize;
    Byte nameBuf[nameSize+1];
    nread = reader.ReadNByte(nameBuf, nameSize);
    name = std::string((char*)nameBuf, nread);
    attrSize += nread;


    return attrSize;
}

void HdlrBox::Init(uint32_t hdlrtype, const std::string& name) {
    this->version = 0;
    memset(flags.data(), 0, flags.size());
    size += 4;

    if (hdlrtype == 0) {
        Byte hType[4]{'v', 'i', 'd', 'e'};
        memmove(&handlerType, hType, 4);
    }
    this->name = name;

    // 1 for '\0'
    size += 20 + name.size() + 1;
}

HdlrBox::HdlrBox() {
    size = 8;
    Byte tp[4]{'h', 'd', 'l', 'r'};
    memmove(&type, tp, 4);
}

void HdlrBox::WriteAttr(FILE *out_file) {
    fwrite(&version, 1, 1, out_file);
    fwrite(flags.data(), 1, 3, out_file);
    Byte predefind[64]{0};
    fwrite(predefind, 1, 4, out_file);
    fwrite(&handlerType, 1, 4, out_file);
    fwrite(predefind, 1, 12, out_file);
    fwrite(name.c_str(), 1, name.size(), out_file);
    fwrite("\0", 1, 1, out_file);
}


