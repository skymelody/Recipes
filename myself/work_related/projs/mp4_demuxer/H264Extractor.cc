//
// Created by skymelody on 2019/12/10.
//

#include "H264Extractor.h"
#include "Mp4Parser.h"
#include "MdatBox.h"
#include "StcoBox.h"
#include "StszBox.h"
#include "StscBox.h"
#include "AvcCBox.h"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
struct SamplePacket {
    int len;
    Byte* data;

    SamplePacket() : len(0), data(nullptr) {}
    explicit SamplePacket(int len) : len(len), data(new Byte[len]) {}
    SamplePacket(SamplePacket&& sample) noexcept {
        len = sample.len;
        data = sample.data;
        sample.len = 0;
        sample.data = nullptr;
    }

    SamplePacket& operator=(SamplePacket&& sample) noexcept {
        len = sample.len;
        if (data != nullptr) {
            delete[] data;
        }
        data = sample.data;
        sample.len = 0;
        sample.data = nullptr;
        return *this;
    }

    SamplePacket(const SamplePacket& sample) = delete;
    SamplePacket& operator=(const SamplePacket&) = delete;
    ~SamplePacket() {
        len = 0;
        delete [] data;
    }
};

bool H264Extractor::Extract(const std::string &inputFilePath, const std::string &outputFilePath) {
    Mp4Parser parser;
    bool res = parser.Parse(inputFilePath);
    cout << res << endl;
    /* parse failed */
    if (!res) {
        return res;
    }
    return DoExtract(parser, outputFilePath);
}

std::vector<SamplePacket> GetSample(FileStreamReader& reader,
        vector<uint32_t> chunkOffset,
        vector<uint32_t> sampleSize,
        vector<ChunkSampleInfo> chunkSampleMap) {

    std::vector<SamplePacket> samples;
    if (chunkOffset.empty() || sampleSize.empty() || chunkSampleMap.empty()) {
        return samples;
    }

    uint32_t offsetBase = chunkOffset[0];
    for (uint32_t& val : chunkOffset) {
        val -= offsetBase;
    }

    vector<uint32_t> sampleOffset;

    int nscmap = chunkSampleMap.size();
    int csidx = 0;
    int sampleIdx = 0;


    int checkSum = 0;
    for (int i = 0; i < chunkOffset.size(); ++i) {

        int nextChunkIdx = (csidx+1 < nscmap ?
                chunkSampleMap[csidx+1].firstChunk : chunkSampleMap[csidx].firstChunk)-1;


        if (i >= nextChunkIdx && csidx+1 < nscmap) {
            ++csidx;
        }

        int nsample = chunkSampleMap[csidx].samplesPerChunk;

        int accuSize = 0;
        for (int j = 0; j < nsample; ++j) {
            sampleOffset.push_back(chunkOffset[i]+accuSize);
            accuSize += (sampleIdx < sampleSize.size() ? sampleSize[sampleIdx] : 0);
            ++sampleIdx;
            ++checkSum;
        }
    }

    if (checkSum != sampleSize.size()) {
        cout << checkSum << "  " << sampleSize.size() << endl;
        cout << "checksum Err" << endl;
        return samples;
    }

    for (int i = 0; i < sampleOffset.size(); ++i) {
        SamplePacket sample(sampleSize[i]);

        reader.SkipNByte(sampleOffset[i]-reader.CurrentOffset());
        int nread = reader.ReadNByte(sample.data, sample.len);
        if (nread != sample.len) {
	    cout << "nread failed" << endl;
            return samples;
        }
        samples.push_back(std::move(sample));

    }

    return samples;
}

bool WriteToDisk(const vector<SamplePacket>& samples, const string& outputPath) {
    int fd = open(outputPath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0664);
    if (fd == -1) {
        return false;
    }
    for (const auto& sample : samples) {
        write(fd, sample.data, sample.len);
    }
    close(fd);
    return true;
}

struct SpsPpsSeen {
    SpsPpsSeen() : newIdr(true), idrSpsSeen(false), idrPpsSeen(false) {}
    bool newIdr;
    bool idrSpsSeen;
    bool idrPpsSeen;
};

struct SpsPpsInfo {
    int spsOffset;
    int ppsOffset;
    vector<uint8_t> spspps;
};

static int SampleGrowBy(SamplePacket& sample, int growBy) {
    int newSize;
    if (sample.len == 0) {
        sample.data = new Byte[growBy];
        sample.len = growBy;
        return 0;
    }
    newSize = sample.len + growBy;
    if (sample.data) {
        sample.data = (Byte*)realloc(sample.data, newSize);
        if (sample.data == nullptr) {
            return -1;
        }
    }
    else {
        sample.data = (Byte*)malloc(newSize);
        if (!sample.data) {
            return -1;
        }
    }
    sample.len += growBy;
    return 0;
}

static int AllocAndCopy(SamplePacket& out, const uint8_t* spsPps, uint32_t spsPpsSize, const uint8_t* in, uint32_t inSize) {
    uint32_t offset = out.len;
    uint8_t nalHeaderSize = offset ? 3 : 4;
    int err = SampleGrowBy(out, spsPpsSize + inSize + nalHeaderSize);
    if (err < 0) {
        return err;
    }
    if (spsPps) {
        memcpy(out.data+offset, spsPps, spsPpsSize);
    }
    memcpy(out.data+spsPpsSize+nalHeaderSize+offset, in, inSize);
    if (!offset) {
        (out.data+spsPpsSize)[0] =
        (out.data+spsPpsSize)[1] = 0;
        (out.data+spsPpsSize)[2] = 0;
        (out.data+spsPpsSize)[3] = 1;
    }
    else {
        (out.data+offset+spsPpsSize)[0] =
        (out.data+offset+spsPpsSize)[1] = 0;
        (out.data+offset+spsPpsSize)[2] = 1;
    }
    return 0;
}

bool DoConvert(SamplePacket& sample, SpsPpsSeen& seen, const SpsPpsInfo& spspps, int prefixLen) {
    uint8_t unitType;
    int32_t nalSize;
    uint32_t cumulSize = 0;
    int lenSize = prefixLen;
    const Byte* buf = sample.data;
    int bufSize = sample.len;
    const Byte* bufEnd = buf + bufSize;
    const auto& spsppsData = spspps.spspps;
    SamplePacket out;


    do {
        if (buf + lenSize > bufEnd) {
            return false;
        }
        int i;
        for (nalSize = 0, i = 0; i < lenSize; ++i) {
            nalSize = (nalSize << 8) | buf[i];
        }
        buf += lenSize;
        unitType = *buf & 0x1f;

        if (nalSize > bufEnd-buf || nalSize < 0) {
            return false;
        }

        if (unitType ==7) {
            seen.idrSpsSeen = seen.newIdr = true;
        }
        else if (unitType == 8) {
            seen.idrPpsSeen = seen.newIdr = true;

            if (!seen.idrSpsSeen) {
                if (spspps.spsOffset == -1) {
                    return false;
                }
                else {
                    if (AllocAndCopy(out, spsppsData.data()+spspps.spsOffset,
                            spspps.ppsOffset != -1 ? spspps.ppsOffset : spsppsData.size()-spspps.spsOffset,
                            buf, nalSize) < 0) {
                        return false;
                    }
                    seen.idrSpsSeen = true;
                    goto nextNal;
                }
            }
        }

        if (!seen.newIdr && unitType == 5 && (buf[1]&0x80)) {
            seen.newIdr = true;
        }

        if (seen.newIdr && unitType == 5 && !seen.idrSpsSeen && !seen.idrPpsSeen) {
            if (AllocAndCopy(out, spsppsData.data(), spsppsData.size(), buf, nalSize) < 0) {
                return false;
            }
            seen.newIdr = false;
        }
        else if (seen.newIdr && unitType == 5 && seen.idrSpsSeen && !seen.idrPpsSeen) {
            if (spspps.ppsOffset == -1) {
                return false;
            }
            else if (AllocAndCopy(out, spsppsData.data()+spspps.ppsOffset, spsppsData.size()-spspps.ppsOffset,
                     buf, nalSize) < 0) {
                return false;
            }
        }
        else {
            if (AllocAndCopy(out, nullptr, 0, buf, nalSize) < 0) {
                return false;
            }
            if (!seen.newIdr && unitType == 1) {
                seen.newIdr = true;
                seen.idrSpsSeen = false;
                seen.idrPpsSeen = false;
            }
        }

nextNal:
        buf += nalSize;
        cumulSize += nalSize+lenSize;
    } while (cumulSize < bufSize);

    std::swap(sample.data, out.data);
    std::swap(sample.len, out.len);

    return true;
}

static bool ConvertToAnnexb(vector<SamplePacket>& samples, AvcCBox* avcC) {
    SpsPpsInfo spsppsInfo;
    auto sps = avcC->GetSps();
    auto pps = avcC->GetPps();
    auto& spspps = spsppsInfo.spspps;

    {
        spsppsInfo.spsOffset = 0;
        for (int i = 0; i < sps.size(); ++i) {
            vector<uint8_t> tmp{0x00, 0x00, 0x00, 0x01};
            tmp.insert(tmp.end(), sps[i].sps.begin(), sps[i].sps.end());
            spspps.insert(spspps.end(), tmp.begin(), tmp.end());
        }
    }

    {
        spsppsInfo.ppsOffset = spspps.size();
        for (int i = 0; i < pps.size(); ++i) {
            vector<uint8_t> tmp{0x00, 0x00, 0x00, 0x01};
            tmp.insert(tmp.end(), pps[i].pps.begin(), pps[i].pps.end());
            spspps.insert(spspps.end(), tmp.begin(), tmp.end());
        }
    }

    SpsPpsSeen seen;

    bool res = false;
    for (int i = 0; i < samples.size(); ++i) {
        res = DoConvert(samples[i], seen, spsppsInfo, avcC->PrefixLen());
        if (!res) {
            break;
        }
    }
    return res;
}


bool H264Extractor::DoExtract(Mp4Parser &parser, const std::string &outputPath) {
    MdatBox* mdat = dynamic_cast<MdatBox*>(parser.FindBox(parser.Boxes(), "mdat"));
    StcoBox* stco = dynamic_cast<StcoBox*>(parser.FindVideoBox(parser.Boxes(), "stco"));
    StszBox* stsz = dynamic_cast<StszBox*>(parser.FindVideoBox(parser.Boxes(), "stsz"));
    StscBox* stsc = dynamic_cast<StscBox*>(parser.FindVideoBox(parser.Boxes(), "stsc"));
    AvcCBox* avcc = dynamic_cast<AvcCBox*>(parser.FindVideoBox(parser.Boxes(), "avcC"));

    if (mdat == nullptr || stco == nullptr || stsz == nullptr || stsc == nullptr | avcc == nullptr) {
        return false;
    }

    FileStreamReader reader(mdat->GetFile());
    vector<uint32_t> chunkOffset = stco->ChunkOffset();
    vector<uint32_t> sampleSize = stsz->EntrySizeVec();
    vector<ChunkSampleInfo> chunkSampleMap = stsc->SampleInfos();

    vector<SamplePacket> samples = GetSample(reader, chunkOffset, sampleSize, chunkSampleMap);
    if (samples.empty()) {
        return false;
    }

    if (!ConvertToAnnexb(samples, avcc)) {
        return false;
    }

    cout << "Sample Size : " << samples.size() << endl;
    if (!WriteToDisk(samples, outputPath)) {
        cout << "write failed" << endl;
        return false;
    }

    return true;
}
