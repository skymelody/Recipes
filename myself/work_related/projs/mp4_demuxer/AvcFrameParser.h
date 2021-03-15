//
// Created by skymelody on 2019/12/14.
//

#ifndef MP4TOH264_AVCFRAMEPARSER_H
#define MP4TOH264_AVCFRAMEPARSER_H

#include <vector>
#include <cstdint>
#include "NaluParser.h"
#include "FileStreamReader.h"
#include "Sample.h"
#include "AvcSliceHeader.h"
#include "SpsPps.h"
#include "NaluParser.h"
#include "Box.h"
#include <memory>

class AvcFrameParser {
public:
    AvcFrameParser();
    ~AvcFrameParser();
    bool Parse(FileStreamReader& reader);
    std::vector<Box*> GetBox() const;

private:
    MuxSps* GetSliceSps(AvcSliceHeader& header);
    bool ParseSample(FileStreamReader& reader, const Nalu& nalu, Sample& sample, bool lastUnit);
    bool ParseSliceHeader(const std::vector<Byte>& data, int type, uint32_t nalRefIdc, AvcSliceHeader& sliceHeader);
    void CheckIfSampleIsCompleted(Sample& sample);
    bool SameFrame(uint32_t naltype1, uint32_t nalRefIdc1, AvcSliceHeader& header1,
                   uint32_t naltype2, uint32_t nalRefIdc2, AvcSliceHeader& header2);

    bool CreateMp4File(FILE* mdatFile,
                       const std::vector<MuxSps*>& spsVec,
                       const std::vector<MuxPps*>& ppsVec,
                       const std::vector<uint32_t>& sampleSize,
                       const std::vector<Sample>& samples);

private:

    std::vector<MuxSps*> spsVec_;
    std::vector<MuxPps*> ppsVec_;
    int naluType_;
    int naluRefIdc_;
    int prevPicOrderCntMsb_;
    uint32_t prevPicOrderCntLsb_;

    uint32_t prevFrameNumOffset_;
    uint32_t prevFrameNum_;
    uint32_t totalAccessUnitCount_;
    uint32_t totalNalUnitCount_;
    uint32_t accessUnitVclNalUnitCount_;

    int nalCount_;
    std::unique_ptr<AvcSliceHeader> sliceHeader_;
    std::vector<Byte> sampleData_;


    std::vector<Box*> boxes_;
};


#endif //MP4TOH264_AVCFRAMEPARSER_H
