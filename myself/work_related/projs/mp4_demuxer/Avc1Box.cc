//
// Created by skymelody on 2019/12/10.
//

#include "Avc1Box.h"
#include "ByteConvert.h"
#include <cstring>

size_t Avc1Box::ParseAttr(FileStreamReader &reader) {
    Byte buf[36];
    int nread = 0;
    int attrSize = 0;
    /* reserved */
    nread = reader.ReadNByte(buf, 6);
    attrSize += nread;

    /* dataRefIndex */
    nread = reader.ReadNByte(buf, 2);
    memmove(&dataRefIndex, buf, nread);
    attrSize += nread;

    /* predefined */
    nread = reader.ReadNByte(buf, 2);
    attrSize += nread;

    /* reserved */
    nread = reader.ReadNByte(buf, 2);
    attrSize += nread;

    /* predefined */
    nread = reader.ReadNByte(buf, 12);
    attrSize += nread;

    /* width */
    nread = reader.ReadNByte(buf, 2);
    memmove(&width, buf, nread);
    attrSize += nread;

    /* height */
    nread = reader.ReadNByte(buf, 2);
    memmove(&height, buf, nread);
    attrSize += nread;

    /* hor resolution */
    nread = reader.ReadNByte(buf, 4);
    memmove(&horResolution, buf, nread);
    attrSize += nread;

    /* ver resolution */
    nread = reader.ReadNByte(buf, 4);
    memmove(&verResolution, buf, nread);
    attrSize += nread;

    /* reserved */
    nread = reader.ReadNByte(buf, 4);
    attrSize += nread;

    /* frame count */
    nread = reader.ReadNByte(buf, 2);
    memmove(&frameCount, buf, nread);
    attrSize += nread;

    /* compressor name */
    nread = reader.ReadNByte(buf, 32);
    compressorName = std::string((char*)buf, nread);
    attrSize += nread;

    /* depth */
    nread = reader.ReadNByte(buf, 2);
    memmove(&depth, buf, nread);
    attrSize += nread;

    /* predefine */
    nread = reader.ReadNByte(buf, 2);
    attrSize += nread;

    return attrSize;

}

void Avc1Box::Init(uint32_t width, uint32_t height) {
    this->width = width;
    this->height = height;
    this->horResolution = 0x00480000;
    this->verResolution = 0x00480000;
    this->frameCount = 1;
    this->compressorName = " AVC Coding";
    this->depth = 0x0018;
    this->dataRefIndex = 1;

    size += 78;
}

Avc1Box::Avc1Box() {
    size = 8;
    Byte tp[4]{'a', 'v', 'c', '1'};
    memmove(&type, tp, 4);
}

void Avc1Box::WriteAttr(FILE *out_file) {
    Byte buf[64]{0};
    fwrite(buf, 1, 6, out_file);

    uint16_t bref = LittleEndToBigEnd(dataRefIndex);
    fwrite(&bref, 1, 2, out_file);

    fwrite(buf, 1, 16, out_file);

    uint16_t bwidth = LittleEndToBigEnd(width);
    fwrite(&bwidth, 1, 2, out_file);

    uint16_t bheight = LittleEndToBigEnd(height);
    fwrite(&bheight, 1, 2, out_file);

    uint32_t bhorResolution = LittleEndToBigEnd(horResolution);
    fwrite(&bhorResolution, 1, 4, out_file);

    uint32_t bverResolution = LittleEndToBigEnd(verResolution);
    fwrite(&bverResolution, 1, 4, out_file);

    fwrite(buf, 1, 4, out_file);

    uint16_t bframeCount = LittleEndToBigEnd(frameCount);
    fwrite(&bframeCount, 1, 2, out_file);

    fwrite(compressorName.c_str(), 1, compressorName.size(), out_file);
    fwrite(buf, 1, 32-compressorName.size(), out_file);


    uint16_t bdepth = LittleEndToBigEnd(depth);
    fwrite(&bdepth, 1, 2, out_file);

    buf[0] = buf[1] = 0xff;
    fwrite(&buf, 1, 2, out_file);
}
