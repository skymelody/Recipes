//
// Created by skymelody on 2019/12/10.
//

#include <cstring>
#include "StscBox.h"
#include "ByteConvert.h"

size_t StscBox::ParseAttr(FileStreamReader &reader) {
    Byte buf[4];
    int attrSize = 0;
    /* version */
    int nread = reader.ReadNByte(buf, 1);
    memmove(&version, buf, nread);
    attrSize += nread;

    /* flags */
    for (int i = 0; i < 3; ++i) {
        nread = reader.ReadNByte(buf, 1);
        memmove(flags.data()+i, buf, nread);
        attrSize += nread;
    }

    /* entry count */
    nread = reader.ReadNByte(buf, 4);
    memmove(&entryCount, buf, nread);
    attrSize += nread;

    int nentry = BytesToInt(entryCount);
    for (int i = 0; i < nentry; ++i) {
        ChunkSampleInfo info {0};

        /* First Chunk*/
        nread = reader.ReadNByte(buf, 4);
        memmove(&info.firstChunk, buf, nread);
        attrSize += nread;

        /* samples per chunk */
        nread = reader.ReadNByte(buf, 4);
        memmove(&info.samplesPerChunk, buf, nread);
        attrSize += nread;

        /* sample descp index */
        nread = reader.ReadNByte(buf, 4);
        memmove(&info.sampleDescriptionIndex, buf, nread);
        attrSize += nread;

        sampleInfos.push_back(info);
    }

    return attrSize;
}

uint32_t StscBox::EntryCount() const {
    return sampleInfos.size();
}

std::vector<ChunkSampleInfo> StscBox::SampleInfos() const {
    std::vector<ChunkSampleInfo> res;
    for (const auto& entry : sampleInfos) {
        res.push_back(ChunkSampleInfo{
            BytesToInt(entry.firstChunk),
            BytesToInt(entry.samplesPerChunk),
            BytesToInt(entry.sampleDescriptionIndex)
        });
    }
    return res;
}

void StscBox::AddEntry(const ChunkSampleInfo &sampleInfo) {
    sampleInfos.push_back(sampleInfo);
    size += 12;
}

StscBox::StscBox() {
    size = 8;
    Byte tp[4]{'s', 't', 's', 'c'};
    memmove(&type, tp, 4);
    version = 0;
    memset(flags.data(), 0, flags.size());
    size += 8;
}

void StscBox::WriteAttr(FILE *out_file) {
    fwrite(&version, 1, 1, out_file);
    fwrite(flags.data(), 1, 3, out_file);
    uint32_t bcount = LittleEndToBigEnd((uint32_t)sampleInfos.size());
    fwrite(&bcount, 1, 4, out_file);
    for (int i = 0; i < sampleInfos.size(); ++i) {
        uint32_t tmp = LittleEndToBigEnd(sampleInfos[i].firstChunk);
        fwrite(&tmp, 1, 4, out_file);
        tmp = LittleEndToBigEnd(sampleInfos[i].samplesPerChunk);
        fwrite(&tmp, 1, 4, out_file);
        tmp = LittleEndToBigEnd(sampleInfos[i].sampleDescriptionIndex);
        fwrite(&tmp, 1, 4, out_file);
    }
}
