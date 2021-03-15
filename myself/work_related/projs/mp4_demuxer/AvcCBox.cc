//
// Created by skymelody on 2019/12/10.
//

#include "AvcCBox.h"
#include "ByteConvert.h"
#include <cstring>
#include <iostream>

size_t AvcCBox::ParseAttr(FileStreamReader &reader) {
    int nread = 0;
    int attrSize = 0;
    Byte buf[4];

    /* version */
    nread = reader.ReadNByte(buf, 1);
    memmove(&version, buf, nread);
    attrSize += nread;

    /* profile */
    nread = reader.ReadNByte(buf, 1);
    memmove(&profile, buf, nread);
    attrSize += nread;

    /* compatibility */
    nread = reader.ReadNByte(buf, 1);
    memmove(&compatibility, buf, 1);
    attrSize += nread;

    /* level */
    nread = reader.ReadNByte(buf, 1);
    memmove(&level, buf, 1);
    attrSize += nread;

    /* lenSizeMinusOne */
    nread = reader.ReadNByte(buf, 1);
    memmove(&lenSizeMinusOne, buf, 1);
    lenSizeMinusOne &= uint8_t(3);
    attrSize += nread;

    /* sps num */
    nread = reader.ReadNByte(buf, 1);
    memmove(&spsNum, buf, 1);
    spsNum &= uint8_t(31);
    attrSize += nread;

    /* sps */
    {
        int nsps = BytesToInt(spsNum);
        for (int i = 0; i < nsps; ++i) {
            /* sps len */
            uint16_t len;
            nread = reader.ReadNByte(buf, 2);
            memmove(&len, buf, nread);
            attrSize += nread;

            /* sps data */
            int spslen = BytesToInt(len);
            std::vector<uint8_t> spsData;
            for (int j = 0; j < spslen; ++j) {
                Byte tmp;
                nread = reader.ReadNByte(buf, 1);
                memmove(&tmp, buf, nread);
                attrSize += nread;
                spsData.push_back(tmp);
            }
            sps.push_back({len, spsData});
        }
    }

    /* pps num */
    nread = reader.ReadNByte(buf, 1);
    memmove(&ppsNum, buf, nread);
    attrSize += nread;
    /* pps */
    {
        int npps = BytesToInt(ppsNum);
        for (int i = 0; i < npps; ++i) {
            /* pps len */
            uint16_t len;
            nread = reader.ReadNByte(buf, 2);
            memmove(&len, buf, nread);
            attrSize += nread;

            /* pps data */
            int ppslen = BytesToInt(len);
            std::vector<uint8_t> ppsData;
            for (int j = 0; j < ppslen; ++j) {
                Byte tmp;
                nread = reader.ReadNByte(buf, 1);
                memmove(&tmp, buf, nread);
                attrSize += nread;
                ppsData.push_back(tmp);
            }
            pps.push_back({len, ppsData});
        }
    }


    return attrSize;
}

std::vector<Sps> AvcCBox::GetSps() const {
    std::vector<Sps> res;
    for (const auto& entry : sps) {
        res.push_back({BytesToInt(entry.len), entry.sps});
    }
    return res;
}

std::vector<Pps> AvcCBox::GetPps() const {
    std::vector<Pps> res;
    for (const auto& entry : pps) {
        res.push_back({BytesToInt(entry.len), entry.pps});
    }
    return res;
}

int AvcCBox::PrefixLen() const {
    return BytesToInt(lenSizeMinusOne)+1;
}

void AvcCBox::Init(uint8_t profile,
                   uint8_t compatibility,
                   uint8_t level,
                   uint8_t lenSize,
                   const std::vector<Sps>& sps,
                   const std::vector<Pps>& pps) {
    this->version = 1;
    this->profile = profile;
    this->compatibility = compatibility;
    this->level = level;
    this->lenSizeMinusOne = lenSize-1;
    this->spsNum = sps.size();
    this->sps = sps;
    this->ppsNum = pps.size();
    this->pps = pps;

    int spsSize = 0;
    int ppsSize = 0;
    for (const auto& entry : sps) {
        spsSize += (2+entry.len);
    }
    for (const auto& entry : pps) {
        ppsSize += (2+entry.len);
    }
    size += 7 + spsSize + ppsSize;
}

AvcCBox::AvcCBox() {
    size = 8;
    Byte tp[4]{'a', 'v', 'c', 'C'};
    memmove(&type, tp, 4);
}

void AvcCBox::WriteAttr(FILE *out_file) {
    fwrite(&version, 1, 1, out_file);
    fwrite(&profile, 1, 1, out_file);
    fwrite(&compatibility, 1, 1, out_file);
    fwrite(&level, 1, 1, out_file);

    lenSizeMinusOne |= 0xfc;
    fwrite(&lenSizeMinusOne, 1, 1, out_file);
    spsNum |= 0xe0;
    fwrite(&spsNum, 1, 1, out_file);

    for (int i = 0; i < sps.size(); ++i) {
        uint16_t len = LittleEndToBigEnd(sps[i].len);
        fwrite(&len, 1, 2, out_file);
        fwrite(sps[i].sps.data(), 1, sps[i].sps.size(), out_file);
    }
    fwrite(&ppsNum, 1, 1, out_file);

    for (int i = 0; i < pps.size(); ++i) {
        uint16_t len = LittleEndToBigEnd(pps[i].len);
        fwrite(&len, 1, 2, out_file);
        fwrite(pps[i].pps.data(), 1, pps[i].pps.size(), out_file);
    }
}

