//
// Created by skymelody on 2019/12/10.
//

#include "StcoBox.h"
#include "ByteConvert.h"
#include <cstring>
#include <iostream>

std::vector<uint32_t> StcoBox::ChunkOffset() const {
    std::vector<uint32_t> res;
    for (uint32_t val : chunkOffset) {
        res.push_back(BytesToInt(val));
    }
    return res;
}

size_t StcoBox::ParseAttr(FileStreamReader &reader) {
    Byte buf[4];
    int nread = 0;
    int attrSize = 0;

    /* version */
    nread = reader.ReadNByte(buf, 1);
    memmove(&version, buf, nread);
    attrSize += nread;

    /* flags */
    nread = reader.ReadNByte(buf, 3);
    memmove(flags.data(), buf, nread);
    attrSize += nread;

    /* n entry count */
    nread = reader.ReadNByte(buf, 4);
    memmove(&entryCount, buf, nread);
    attrSize += nread;

    /* chunk offset */
    int n = BytesToInt(entryCount);
    for (int i = 0; i < n; ++i) {
        uint32_t offset = 0;
        nread = reader.ReadNByte(buf, 4);
        memmove(&offset, buf, nread);
        chunkOffset.push_back(offset);
        attrSize += nread;
    }

    return attrSize;
}

void StcoBox::AddEntry(uint32_t offset) {
    chunkOffset.push_back(offset);
    size += 4;
}

StcoBox::StcoBox() {
    size = 8;
    Byte tp[4]{'s', 't', 'c', 'o'};
    memmove(&type, tp, 4);
    version = 0;
    memset(flags.data(), 0, flags.size());
    size += 8;
}

void StcoBox::WriteAttr(FILE *out_file) {
    fwrite(&version, 1, 1, out_file);
    fwrite(flags.data(), 1, 3, out_file);
    uint32_t bcount = LittleEndToBigEnd((uint32_t)chunkOffset.size());
    fwrite(&bcount, 1, 4, out_file);

    int curOffset = ftell(out_file);
    for (int i = 0; i < chunkOffset.size(); ++i) {
        uint32_t tmp = LittleEndToBigEnd((uint32_t)chunkOffset[i]);
        fwrite(&tmp, 1, 4, out_file);
    }
}

void StcoBox::AdjustChunkOffset(uint32_t mdatoffset) {
    for (uint32_t& entry : chunkOffset) {
        entry += mdatoffset;
    }
}

