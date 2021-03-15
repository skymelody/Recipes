//
// Created by skymelody on 2019/12/10.
//

#include "StszBox.h"
#include "ByteConvert.h"
#include <cstring>
using namespace std;


size_t StszBox::ParseAttr(FileStreamReader &reader) {
    Byte buf[4];
    int nread = 0;
    int attrSize = 0;

    /* Version */
    nread = reader.ReadNByte(buf, 1);
    memmove(&version, buf, nread);
    attrSize += nread;

    /* flags */
    nread = reader.ReadNByte(buf, 3);
    memmove(flags.data(), buf, nread);
    attrSize += nread;

    /* sample size */
    nread = reader.ReadNByte(buf, 4);
    memmove(&sampleSize, buf, nread);
    attrSize += nread;

    /* sample count */
    nread = reader.ReadNByte(buf, 4);
    memmove(&sampleCount, buf, nread);
    attrSize += nread;

    if (BytesToInt(sampleSize) == 0) {
        int n = BytesToInt(sampleCount);
        for (int i = 0; i < n; ++i) {
            nread = reader.ReadNByte(buf, 4);
            uint32_t tmp;
            memmove(&tmp, buf, nread);
            attrSize += nread;
            entrySizeVec.push_back(tmp);
        }
    }

    return attrSize;
}

std::vector<uint32_t> StszBox::EntrySizeVec() const {
    std::vector<uint32_t> res;
    for (uint32_t val : entrySizeVec) {
        res.push_back(BytesToInt(val));
    }
    return res;
}

void StszBox::Init(const std::vector<uint32_t> &sampleSizes) {
    version = 0;
    memset(flags.data(), 0, flags.size());

    sampleSize = 0;
    sampleCount = sampleSizes.size();
    entrySizeVec = sampleSizes;

    size += 12 * 4*sampleSizes.size();
}

void StszBox::AddEntry(uint32_t size) {
    entrySizeVec.push_back(size);
    this->size += 4;
}

StszBox::StszBox() {
    size = 8;
    Byte tp[4]{'s', 't', 's', 'z'};
    memmove(&type, tp, 4);
    version = 0;
    memset(flags.data(), 0, flags.size());
    sampleSize = 0;
    size += 12;
}

void StszBox::WriteAttr(FILE *out_file) {
    fwrite(&version, 1, 1, out_file);
    fwrite(flags.data(), 1, 3, out_file);
    uint32_t bsize = LittleEndToBigEnd(sampleSize);
    fwrite(&bsize, 1, 4, out_file);
    uint32_t bcount = LittleEndToBigEnd((uint32_t)entrySizeVec.size());
    fwrite(&bcount, 1, 4, out_file);
    if (sampleSize == 0) {
        for (int i = 0; i < entrySizeVec.size(); ++i) {
            uint32_t tmp = LittleEndToBigEnd((uint32_t)entrySizeVec[i]);
            fwrite(&tmp, 1, 4, out_file);
        }
    }

}
