//
// Created by skymelody on 2019/12/14.
//

#include "AvcFrameParser.h"
#include "SpsPps.h"
#include "BitStream.h"
#include "NaluParser.h"
#include "ByteConvert.h"
#include "MdatBox.h"
#include "BoxCreater.h"
#include "Avc1Box.h"
#include "AvcCBox.h"
#include "StblBox.h"
#include "MinfBox.h"
#include "HdlrBox.h"
#include "MvhdBox.h"
#include "MdiaBox.h"
#include "VmhdBox.h"
#include "TrakBox.h"
#include "MoovBox.h"
#include "StsdBox.h"
#include "StcoBox.h"
#include "StszBox.h"
#include "CttsBox.h"
#include "StssBox.h"
#include "StscBox.h"
#include "SttsBox.h"
#include "TkhdBox.h"
#include "MdhdBox.h"
#include "UrlBox.h"
#include "DrefBox.h"
#include "DinfBox.h"
#include "FtypBox.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;


const unsigned int SLICE_TYPE_P                                    = 0;
const unsigned int SLICE_TYPE_B                                    = 1;
const unsigned int SLICE_TYPE_I                                    = 2;
const unsigned int SLICE_TYPE_SP                                   = 3;
const unsigned int SLICE_TYPE_SI                                   = 4;



bool AvcFrameParser::ParseSample(FileStreamReader &reader, const Nalu& nalu, Sample& sample, bool lastUnit){
    const auto& naludata = nalu.data;

    if (!naludata.empty()) {
        uint32_t nalutype = naludata[0]&0x1f;
        uint32_t nalurefIdc = (naludata[0]>>5)&3;

        if (nalutype == NALU_TYPE_AUD) {
            uint32_t primaryPicType = (naludata[1] >> 5);
            CheckIfSampleIsCompleted(sample);
        }
        else if (nalutype == NALU_TYPE_SLICE ||
                 nalutype == NALU_TYPE_IDR ||
                 nalutype == NALU_TYPE_DPA) {
            AvcSliceHeader* header = new AvcSliceHeader;
            bool res = ParseSliceHeader(std::vector<Byte>(naludata.begin()+1, naludata.end()), nalutype, nalurefIdc, *header);
            if (!res) {
                return false;
            }
            if (header != nullptr) {
                if (sliceHeader_ &&
                    !SameFrame(naluType_, naluRefIdc_, *sliceHeader_, nalutype, nalurefIdc, *header)) {
                    CheckIfSampleIsCompleted(sample);
                    accessUnitVclNalUnitCount_ = 1;
                }
                else {
                    ++accessUnitVclNalUnitCount_;
                }
            }

            // cout << "nal data : " << naludata.size() << endl;
            uint32_t sizeHex = LittleEndToBigEnd((uint32_t)naludata.size());
            Byte size[4];
            memmove(size, &sizeHex, 4);
            sampleData_.insert(sampleData_.end(), size, size+4);
            sampleData_.insert(sampleData_.end(), naludata.begin(), naludata.end());
            ++nalCount_;
            sliceHeader_.reset(header);
            naluType_ = nalutype;
            naluRefIdc_ = nalurefIdc;
        }
        else if (nalutype == NALU_TYPE_PPS) {
            MuxPps* pps = new MuxPps;
            bool res = ParsePps(naludata.data(), naludata.size(), *pps);
            if (!res) {
                cout << "failed pps " << endl;

                delete pps;
            }
            else {
                delete ppsVec_[pps->pic_parameter_set_id];

                ppsVec_[pps->pic_parameter_set_id] = pps;

                uint32_t sizeHex = LittleEndToBigEnd((uint32_t)naludata.size());
                Byte size[4];
                memmove(size, &sizeHex, 4);
                sampleData_.insert(sampleData_.end(), size, size+4);
                sampleData_.insert(sampleData_.end(), naludata.begin(), naludata.end());
                ++nalCount_;
                CheckIfSampleIsCompleted(sample);
            }
        }
        else if (nalutype == NALU_TYPE_SPS) {
            MuxSps* sps = new MuxSps;
            bool res = ParseSps(naludata.data(), naludata.size(), *sps);
            if (!res) {
                cout << "failed sps " << endl;
                delete sps;
            }
            else {
                delete spsVec_[sps->seq_parameter_set_id];
                spsVec_[sps->seq_parameter_set_id] = sps;
                CheckIfSampleIsCompleted(sample);
            }
        }
        else if (nalutype == NALU_TYPE_SEI) {
            uint32_t sizeHex = LittleEndToBigEnd((uint32_t)naludata.size());
            Byte size[4];
            memmove(size, &sizeHex, 4);
            sampleData_.insert(sampleData_.end(), size, size+4);
            sampleData_.insert(sampleData_.end(), naludata.begin(), naludata.end());
            ++nalCount_;
            CheckIfSampleIsCompleted(sample);
        }
        else {
            CheckIfSampleIsCompleted(sample);
        }
        ++totalNalUnitCount_;
    }

    if (lastUnit && sample.nalCnt == 0) {
        CheckIfSampleIsCompleted(sample);
    }
    return true;
}

void SortSample(pair<uint32_t, uint32_t>* sampleOrder, int n) {
    if (n < 2) {
        return;
    }
    const auto& pivot = sampleOrder[n/2];
    auto* left = sampleOrder;
    auto* right = sampleOrder+n-1;
    while (left <= right) {
        if (left->second < pivot.second) {
            ++left;
            continue;
        }
        if (right->second > pivot.second) {
            --right;
            continue;
        }
        auto tmp = *left;
        *left++ = *right;
        *right-- = tmp;
    }
    SortSample(sampleOrder, (uint32_t)(right-sampleOrder+1));
    SortSample(left, (uint32_t)(sampleOrder+n-left));
}

bool AvcFrameParser::Parse(FileStreamReader &reader) {
    NaluParser naluParser;
    vector<pair<uint32_t, uint32_t>> sampleOrder;

    vector<uint32_t> sampleSize;
    FILE* mdatFile = tmpfile();
    std::vector<Sample> samples;
    for (;;) {
        bool foundSample = false;
        bool res = false;
        do {
            Nalu nalu;
            Sample sample;
            foundSample = false;
            res = naluParser.GetOneNalu(nalu, reader);
            bool lastNalu = false;
            if (!reader.HaveBytes()) {
                lastNalu = true;
            }

            ParseSample(reader, nalu, sample, lastNalu);

            if (sample.nalCnt != 0) {
                foundSample = true;

                fwrite(sample.data.data(), 1, sample.data.size(), mdatFile);


                {
                    int n = samples.size();
                    sample.offset = 0;
                    sample.duration = 1000;
                    sample.descriptionIndex = 0;
                    sample.dts = 0;
                    sample.ctsDelta = 0;
                    if (n > 0) {
                        Sample& prevSample = samples[n-1];
                        int dts = prevSample.dts + prevSample.duration;
                        sample.dts = dts;
                        sample.offset = prevSample.offset + prevSample.data.size();
                    }
                }
                samples.push_back(sample);
                sampleOrder.emplace_back(sample.decodeOrder, sample.displayOrder);
                sampleSize.push_back(sample.data.size());

            }
        }
        while (res);
        if (!res) {
            break;
        }
    }


    int n = sampleOrder.size();
    if (sampleOrder.size() > 1) {
        uint32_t start = 0;
        for (uint32_t i = 0; i <= sampleOrder.size(); ++i) {
            if (i == sampleOrder.size() || sampleOrder[i].second == 0) {
                sort(sampleOrder.begin()+start, sampleOrder.begin()+i,
                        [](const pair<uint32_t, uint32_t>& p1,
                           const pair<uint32_t, uint32_t>& p2) {
                    return p1.second == p2.second ? p1.first < p2.first : p1.second < p2.second;
                });
                start = i;
            }
        }
    }

    uint32_t maxDelta = 0;
    for (uint32_t i = 0; i < sampleOrder.size(); ++i) {
        if (sampleOrder[i].first > i) {
            uint32_t delta = sampleOrder[i].first-i;
            maxDelta = max(maxDelta, delta);
        }
    }

    // ctts
    for (uint32_t i = 0; i < sampleOrder.size(); ++i) {
        uint32_t cts = 1000 * (i+maxDelta);
        Sample& sp = samples[sampleOrder[i].first];
        sp.ctsDelta = cts > sp.dts ? (cts-sp.dts) : 0;
    }



    //***************************************************************************
    // construct mp4
    return CreateMp4File(mdatFile, spsVec_, ppsVec_, sampleSize, samples);

}

static uint64_t ConvertTime(uint64_t timeVal, uint32_t fromTimeScale, uint32_t toTimeScale) {
    if (fromTimeScale == 0) {
        return 0;
    }
    double ratio = (double)toTimeScale / (double) fromTimeScale;
    return (uint64_t)(0.5+(double)timeVal*ratio);
}

static void BuildStbl(StblBox* stbl,
                      StsdBox* stsd,
                      SttsBox* stts,
                      StscBox* stsc,
                      StszBox* stsz,
                      CttsBox* ctts,
                      StcoBox* stco,
                      StssBox* stss,
                      const std::vector<Sample>& samples) {
    uint32_t currentChunkIndex = 0;
    uint32_t currentChunkSize = 0;
    uint64_t currentChunkOffset = 0;
    uint32_t currentSamplesInChunk = 0;
    uint32_t currentSampleDescIndex = 0;
    uint32_t currentDuration = 0;
    uint32_t currentDurationRun = 0;
    uint32_t currentCtsDelta = 0;
    uint32_t currentCtsDeltaRun = 0;


    bool allSamplesAreSunc = false;
    uint32_t sampleCount = samples.size();
    for (int i = 0; i < sampleCount; ++i) {
        const Sample& sample = samples[i];

        uint32_t newDuration = sample.duration;
        if (newDuration != currentDuration && currentDurationRun != 0) {
            stts->AddEntry(currentDurationRun, currentDuration);

            currentDurationRun = 0;
        }
        ++currentDurationRun;
        currentDuration = newDuration;

        uint32_t newCtsDelta = sample.ctsDelta;
        if (newCtsDelta != currentCtsDelta && currentCtsDeltaRun != 0) {
            ctts->AddEntry(currentCtsDeltaRun, currentCtsDelta);
            currentCtsDeltaRun = 0;
        }
        ++currentCtsDeltaRun;
        currentCtsDelta = newCtsDelta;

        stsz->AddEntry(sample.data.size());

        if (sample.isIdr) {
            stss->AddEntry(i+1);
            if (i == 0) {
                allSamplesAreSunc = true;
            }
        }
        else {
            allSamplesAreSunc = false;
        }


    }
    if (sampleCount) {
        stts->AddEntry(currentDurationRun, currentDuration);
    }
    if (ctts) {
        ctts->AddEntry(currentCtsDeltaRun, currentCtsDelta);
    }

    const int SAMPLES_PER_CHUNK = 10;
    int curChunkNumber = 0;
    int sampleAccumulate = 0;
    uint32_t chunkOffset = 0;
    int start = 0;
    for (int i = 0; i < samples.size(); ++i) {
        if ((i+1)%SAMPLES_PER_CHUNK == 0) {
            ChunkSampleInfo info {0};
            info.firstChunk = curChunkNumber+1;
            info.samplesPerChunk = SAMPLES_PER_CHUNK;
            info.sampleDescriptionIndex = samples[i].descriptionIndex + 1;
            sampleAccumulate += SAMPLES_PER_CHUNK;
            stsc->AddEntry(info);
            stco->AddEntry(chunkOffset);
            ++curChunkNumber;
            for (int j = start; j <= i; ++j) {
                chunkOffset += samples[j].data.size();
            }
            start = i+1;
        }
    }
    // last chunk;;
    if (sampleAccumulate != samples.size()) {
        ChunkSampleInfo info {0};
        info.firstChunk = curChunkNumber+1;
        info.samplesPerChunk = samples.size()-sampleAccumulate;
        info.sampleDescriptionIndex = samples[samples.size()-1].descriptionIndex+1;
        stsc->AddEntry(info);

    }
    stco->AddEntry(chunkOffset);


    stbl->AddChild(stsd);
    stbl->AddChild(stts);
    stbl->AddChild(ctts);
    stbl->AddChild(stsc);
    stbl->AddChild(stsz);
    if (!allSamplesAreSunc && stss->EntryCount() != 0) {
        stbl->AddChild(stss);
    }
    else {
        delete stts;
    }
    stbl->AddChild(stco);


}
bool AvcFrameParser::CreateMp4File(FILE* mdatFile,
                          const vector<MuxSps*>& spsVec,
                          const vector<MuxPps*>& ppsVec,
                          const vector<uint32_t>& sampleSize,
                          const std::vector<Sample>& samples) {
    MuxSps* sps = nullptr;
    for (int i = 0; i < SPS_MAX_ID; ++i) {
        if (spsVec[i]) {
            sps = spsVec[i];
            break;
        }
    }
    if (sps == nullptr) {
        return false;
    }
    // video height and width;
    uint32_t videoWidth = 0;
    uint32_t videoHeight = 0;
    sps->GetWidthAndHeight(videoWidth, videoHeight);

    vector<Sps> spsdata;
    for (int i = 0; i <= SPS_MAX_ID; ++i) {
        if (spsVec[i]) {
            Sps sps;
            sps.len = spsVec[i]->data.size();
            sps.sps = spsVec[i]->data;
            spsdata.push_back(sps);
        }
    }
    vector<Pps> ppsdata;
    for (int i = 0; i <= PPS_MAX_ID; ++i) {
        if (ppsVec[i]) {
            Pps pps;
            pps.len = ppsVec[i]->data.size();
            pps.pps = ppsVec[i]->data;
            ppsdata.push_back(pps);
        }
    }

    uint32_t movieTimeScale = 1000;
    uint32_t mediaTimeScale = 24*1000;
    uint64_t videoTrackDuration = ConvertTime(1000*sampleSize.size(), mediaTimeScale, movieTimeScale);
    uint64_t videoMediaDuration = 1000*sampleSize.size();
    FtypBox* ftyp = dynamic_cast<FtypBox*>(NewBox("ftyp"));
    AvcCBox* avcc = dynamic_cast<AvcCBox*>(NewBox("avcC"));
    Avc1Box* avc1 = dynamic_cast<Avc1Box*>(NewBox("avc1"));
    MinfBox* minf = dynamic_cast<MinfBox*>(NewBox("minf"));
    TrakBox* trak = dynamic_cast<TrakBox*>(NewBox("trak"));
    TkhdBox* tkhd = dynamic_cast<TkhdBox*>(NewBox("tkhd"));
    MdiaBox* mdia = dynamic_cast<MdiaBox*>(NewBox("mdia"));
    HdlrBox* hdlr = dynamic_cast<HdlrBox*>(NewBox("hdlr"));
    MdhdBox* mdhd = dynamic_cast<MdhdBox*>(NewBox("mdhd"));
    MoovBox* moov = dynamic_cast<MoovBox*>(NewBox("moov"));
    VmhdBox* vmhd = dynamic_cast<VmhdBox*>(NewBox("vmhd"));
    UrlBox* url = dynamic_cast<UrlBox*>(NewBox("url "));
    DrefBox* dref = dynamic_cast<DrefBox*>(NewBox("dref"));
    DinfBox* dinf = dynamic_cast<DinfBox*>(NewBox("dinf"));

    StszBox* stsz = dynamic_cast<StszBox*>(NewBox("stsz"));
    StsdBox* stsd = dynamic_cast<StsdBox*>(NewBox("stsd"));
    StblBox* stbl = dynamic_cast<StblBox*>(NewBox("stbl"));
    SttsBox* stts = dynamic_cast<SttsBox*>(NewBox("stts"));
    StscBox* stsc = dynamic_cast<StscBox*>(NewBox("stsc"));
    StcoBox* stco = dynamic_cast<StcoBox*>(NewBox("stco"));
    CttsBox* ctts = dynamic_cast<CttsBox*>(NewBox("ctts"));
    StssBox* stss = dynamic_cast<StssBox*>(NewBox("stss"));
    MvhdBox* mvhd = dynamic_cast<MvhdBox*>(NewBox("mvhd"));
    MdatBox* mdat = dynamic_cast<MdatBox*>(NewBox("mdat"));

    mdat->SetFile(mdatFile);


    dref->Init(url, 1);
    avcc->Init(sps->profile_idc,
               sps->constraint_set0_flag<<7 | sps->constraint_set1_flag<<6 | sps->constraint_set2_flag<<5 | sps->constraint_set3_flag<<4,
               sps->level_idc,
               4,
               spsdata,
               ppsdata);


    ftyp->Init("mp42", 1);
    ftyp->AddCompatibleBrand("isom");
    ftyp->AddCompatibleBrand("mp42");
    ftyp->AddCompatibleBrand("avc1");
    avc1->Init(videoWidth, videoHeight);
    avc1->AddChild(avcc);
    stsd->Init(1, avc1);
    BuildStbl(stbl, stsd, stts, stsc, stsz, ctts, stco, stss, samples);

    hdlr->Init(0, "Bento4 Video Handler");
    dinf->AddChild(dref);
    minf->AddChild(vmhd);
    minf->AddChild(dinf);
    minf->AddChild(stbl);
    mdhd->Init(0, 0, mediaTimeScale, videoMediaDuration, "und");

    mdia->AddChild(mdhd);
    mdia->AddChild(hdlr);
    mdia->AddChild(minf);

    uint64_t videoLen = ConvertTime(videoTrackDuration, movieTimeScale, mediaTimeScale);
    mvhd->Init(0, 0, mediaTimeScale, videoLen);
    tkhd->Init(0, 0, 0, videoLen, 0, videoWidth<<16, videoHeight<<16);
    trak->AddChild(tkhd);
    trak->AddChild(mdia);
    moov->AddChild(mvhd);
    moov->AddChild(trak);


    // 8 for mdat header size
    stco->AdjustChunkOffset(ftyp->Size()+moov->Size()+8);

    boxes_.push_back(ftyp);
    boxes_.push_back(moov);
    boxes_.push_back(mdat);


    return true;
}

void AvcFrameParser::CheckIfSampleIsCompleted(Sample &sample) {
    if (sliceHeader_ == nullptr) {
        return;
    }
    if (accessUnitVclNalUnitCount_ == 0) {
        return;
    }
    accessUnitVclNalUnitCount_ = 0;

    MuxSps* sps = GetSliceSps(*sliceHeader_);
    if (sps == nullptr) {
        return;
    }
    int maxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4);

    enum {
        PIC_TYPE_FRAME,
        PIC_TYPE_TOP_FIELD,
        PIC_TYPE_BOTTOM_FILED
    } picType;

    if (sps->frame_mbs_only_flag || !sliceHeader_->field_pic_flag) {
        picType = PIC_TYPE_FRAME;
    }
    else if (sliceHeader_->bottom_field_flag) {
        picType = PIC_TYPE_BOTTOM_FILED;
    }
    else {
        picType = PIC_TYPE_TOP_FIELD;
    }
    int topFieldPicOrderCnt = 0;
    int bottomFieldPicOrderCnt = 0;
    uint32_t frameNumOffset = 0;
    uint32_t frameNum = sliceHeader_->frame_num;

    if (naluType_ == NALU_TYPE_IDR) {
        prevPicOrderCntLsb_ = 0;
        prevPicOrderCntMsb_ = 0;
    }
    else {
        if (frameNum < prevFrameNum_) {
            frameNumOffset = prevFrameNumOffset_ + maxFrameNum;
        }
        else {
            frameNumOffset = prevFrameNumOffset_;
        }
    }

    int picOrderCntMsb = 0;
    if (sps->pic_order_cnt_type == 0) {
        uint32_t maxPicOrderCntLsb = 1 << (sps->log2_max_pic_order_cnt_lsb_minus4 + 4);
        if (sliceHeader_->pic_order_cnt_lsb < prevPicOrderCntLsb_ &&
            prevPicOrderCntLsb_ - sliceHeader_->pic_order_cnt_lsb >= maxPicOrderCntLsb / 2) {
            picOrderCntMsb = prevPicOrderCntMsb_ + maxPicOrderCntLsb;
        } else if (sliceHeader_->pic_order_cnt_lsb > prevPicOrderCntLsb_ &&
                   sliceHeader_->pic_order_cnt_lsb - prevPicOrderCntLsb_ > maxPicOrderCntLsb / 2) {
            picOrderCntMsb = prevPicOrderCntMsb_ - maxPicOrderCntLsb;
        } else {
            picOrderCntMsb = prevPicOrderCntMsb_;
        }

        if (picType != PIC_TYPE_BOTTOM_FILED) {
            topFieldPicOrderCnt = picOrderCntMsb + sliceHeader_->pic_order_cnt_lsb;
        }
        if (picType != PIC_TYPE_TOP_FIELD) {
            if (!sliceHeader_->field_pic_flag) {
                bottomFieldPicOrderCnt = topFieldPicOrderCnt + sliceHeader_->delta_pic_order_cnt[0];
            } else {
                bottomFieldPicOrderCnt = picOrderCntMsb + sliceHeader_->pic_order_cnt_lsb;
            }
        }

    }
    else if (sps->pic_order_cnt_type == 1) {
        uint32_t absFrameNum = 0;
        if (sps->num_ref_frames_in_pic_order_cnt_cycle) {
            absFrameNum = frameNumOffset + frameNum;
        }
        if (naluRefIdc_ == 0 && absFrameNum > 0) {
            --absFrameNum;
        }

        int expectedPicOrderCnt = 0;
        if (absFrameNum > 0) {
            uint32_t picOrderCntCycleCnt = (absFrameNum-1)/sps->num_ref_frames_in_pic_order_cnt_cycle;
            uint32_t frameNumInPicOrderCntCycle = (absFrameNum-1)%sps->num_ref_frames_in_pic_order_cnt_cycle;

            int expectedDeltaPerPicOrderCntCycle = 0;
            for (uint32_t i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; ++i) {
                expectedDeltaPerPicOrderCntCycle += sps->offset_for_ref_frame[i];
            }
            expectedPicOrderCnt = picOrderCntCycleCnt * expectedDeltaPerPicOrderCntCycle;
            for (uint32_t i = 0; i < frameNumInPicOrderCntCycle; ++i) {
                expectedPicOrderCnt += sps->offset_for_ref_frame[i];
            }
        }
        if (naluRefIdc_ == 0) {
            expectedPicOrderCnt += sps->offset_for_non_ref_pic;
        }
        if (!sliceHeader_->field_pic_flag) {
            topFieldPicOrderCnt = expectedPicOrderCnt + sliceHeader_->delta_pic_order_cnt[0];
            bottomFieldPicOrderCnt = topFieldPicOrderCnt + sps->offset_for_top_to_bottom_field + sliceHeader_->delta_pic_order_cnt[1];
        }
        else if (!sliceHeader_->bottom_field_flag) {
            topFieldPicOrderCnt = expectedPicOrderCnt + sliceHeader_->delta_pic_order_cnt[0];
        }
        else {
            bottomFieldPicOrderCnt = expectedPicOrderCnt + sps->offset_for_top_to_bottom_field + sliceHeader_->delta_pic_order_cnt[0];
        }
    }
    else if (sps->pic_order_cnt_type == 2) {
        int picOrderCnt;
        if (naluType_ == NALU_TYPE_IDR) {
            picOrderCnt = 0;
        }
        else if (naluRefIdc_ == 0) {
            picOrderCnt = 2*(frameNumOffset+frameNum)-1;
        }
        else {
            picOrderCnt = 2*(frameNumOffset+frameNum);
        }

        if (!sliceHeader_->field_pic_flag) {
            topFieldPicOrderCnt = picOrderCnt;
            bottomFieldPicOrderCnt = picOrderCnt;
        }
        else if (sliceHeader_->bottom_field_flag) {
            bottomFieldPicOrderCnt = picOrderCnt;
        }
        else {
            topFieldPicOrderCnt = picOrderCnt;
        }
    }

    uint32_t picOrderCnt;
    if (picType == PIC_TYPE_FRAME) {
        picOrderCnt = topFieldPicOrderCnt < bottomFieldPicOrderCnt ? topFieldPicOrderCnt : bottomFieldPicOrderCnt;
    }
    else if (picType == PIC_TYPE_TOP_FIELD) {
        picOrderCnt = topFieldPicOrderCnt;
    }
    else {
        picOrderCnt = bottomFieldPicOrderCnt;
    }

    sample.data = sampleData_;
    sample.nalCnt = nalCount_;
    sample.isIdr = (naluType_ == NALU_TYPE_IDR);
    sample.decodeOrder = totalAccessUnitCount_;
    sample.displayOrder = picOrderCnt;
    sampleData_.clear();
    nalCount_ = 0;
    ++totalAccessUnitCount_;

    prevFrameNum_ = frameNum;
    prevFrameNumOffset_ = frameNumOffset;
    if (naluRefIdc_) {
        prevPicOrderCntMsb_ = picOrderCntMsb;
        prevPicOrderCntLsb_ = sliceHeader_->pic_order_cnt_lsb;
    }
}

AvcFrameParser::AvcFrameParser() :
    accessUnitVclNalUnitCount_(0),
    naluType_(0),
    naluRefIdc_(0),
    prevPicOrderCntLsb_(0),
    prevPicOrderCntMsb_(0),
    prevFrameNum_(0),
    prevFrameNumOffset_(0),
    totalNalUnitCount_(0),
    totalAccessUnitCount_(0),
    nalCount_(0)
    {
    spsVec_.resize(SPS_MAX_ID+1, nullptr);
    ppsVec_.resize(PPS_MAX_ID+1, nullptr);
}

MuxSps *AvcFrameParser::GetSliceSps(AvcSliceHeader& header) {
    MuxPps* pps = ppsVec_[header.pic_parameter_set_id];
    if (pps == nullptr) {
        return nullptr;
    }
    return spsVec_[pps->seq_parameter_set_id];

}


bool AvcFrameParser::ParseSliceHeader(const std::vector<Byte>& data,
                                      int nalutype,
                                      uint32_t naluRefIdc,
                                      AvcSliceHeader &sliceHeader) {
    std::vector<Byte> unescaped = data;
    Unescape(unescaped);
    BitStream bits(unescaped);
    int sz = unescaped.size();

    sliceHeader.size = 0;
    sliceHeader.first_mb_in_slice = ReadGolomb(bits);
    sliceHeader.slice_type = ReadGolomb(bits);
    sliceHeader.pic_parameter_set_id = ReadGolomb(bits);

    if (sliceHeader.pic_parameter_set_id > PPS_MAX_ID) {
        return false;
    }
    const MuxPps* const pps = ppsVec_[sliceHeader.pic_parameter_set_id];
    if (pps == nullptr) {
        return false;
    }
    const MuxSps* const sps = spsVec_[pps->seq_parameter_set_id];

    if (sps == nullptr) {
        return false;
    }
    if (sps->separate_colour_plane_flag) {
        sliceHeader.colour_plane_id = bits.ReadBits(2);
    }
    sliceHeader.frame_num = bits.ReadBits(sps->log2_max_frame_num_minus4+4);
    if (!sps->frame_mbs_only_flag) {
        sliceHeader.field_pic_flag = bits.ReadBit();
        if (sliceHeader.field_pic_flag) {
            sliceHeader.bottom_field_flag = bits.ReadBit();
        }
    }

    int ln = bits.GetBitsRead();
    if (nalutype == NALU_TYPE_IDR) {
        sliceHeader.idr_pic_id = ReadGolomb(bits);
    }
    if (sps->pic_order_cnt_type == 0) {
        sliceHeader.pic_order_cnt_lsb = bits.ReadBits(sps->log2_max_pic_order_cnt_lsb_minus4+4);
        if (pps->pic_order_present_flag && !sliceHeader.field_pic_flag) {
            sliceHeader.delta_pic_order_cnt[0] = SignedGolomb(ReadGolomb(bits));
        }
    }
    if (sps->pic_order_cnt_type == 1 && !sps->delta_pic_order_always_zero_flags) {
        sliceHeader.delta_pic_order_cnt[0] = SignedGolomb(ReadGolomb(bits));
        if (pps->pic_order_present_flag && !sliceHeader.field_pic_flag) {
            sliceHeader.delta_pic_order_cnt[1] = SignedGolomb(ReadGolomb(bits));
        }
    }
    if (pps->redundant_pic_cnt_present_flag) {
        sliceHeader.redundant_pic_cnt = ReadGolomb(bits);
    }

    uint32_t sliceType = sliceHeader.slice_type % 5;
    if (sliceType == SLICE_TYPE_B) {
        sliceHeader.direct_spatial_mv_pred_flag = bits.ReadBit();
    }
    if (sliceType == SLICE_TYPE_P ||
        sliceType == SLICE_TYPE_SP ||
        sliceType == SLICE_TYPE_B) {
        sliceHeader.num_ref_idx_active_override_flag = bits.ReadBit();

        if (sliceHeader.num_ref_idx_active_override_flag) {
            sliceHeader.num_ref_idx_l0_active_minus1 = ReadGolomb(bits);
            if ((sliceHeader.slice_type % 5) == SLICE_TYPE_B) {
                sliceHeader.num_ref_idx_l1_active_minus1 = ReadGolomb(bits);
            }
        }
        else {
            sliceHeader.num_ref_idx_l0_active_minus1 = pps->num_ref_idx_10_active_minus1;
            sliceHeader.num_ref_idx_l1_active_minus1 = pps->num_ref_idx_11_active_minus1;
        }
    }

    if ((sliceHeader.slice_type % 5) != 2 && (sliceHeader.slice_type % 5) != 4) {
        sliceHeader.ref_pic_list_reordering_flag_l0 = bits.ReadBit();
        if (sliceHeader.ref_pic_list_reordering_flag_l0) {
            do {
                sliceHeader.reordering_of_pic_nums_idc = ReadGolomb(bits);
                if (sliceHeader.reordering_of_pic_nums_idc == 0 ||
                    sliceHeader.reordering_of_pic_nums_idc == 1) {
                    sliceHeader.abs_diff_pic_num_minus1 = ReadGolomb(bits);
                }
                else if (sliceHeader.reordering_of_pic_nums_idc == 2) {
                    sliceHeader.long_term_pic_num = ReadGolomb(bits);
                }
            }
            while (sliceHeader.reordering_of_pic_nums_idc != 3);
        }
    }
    if ((sliceHeader.slice_type % 5) == 1) {
        sliceHeader.ref_pic_list_reordering_flag_l1 = bits.ReadBit();
        if (sliceHeader.ref_pic_list_reordering_flag_l1) {
            do {
                sliceHeader.reordering_of_pic_nums_idc = ReadGolomb(bits);
                if (sliceHeader.reordering_of_pic_nums_idc == 0 ||
                    sliceHeader.reordering_of_pic_nums_idc == 1) {
                    sliceHeader.abs_diff_pic_num_minus1 = ReadGolomb(bits);
                }
                else if (sliceHeader.reordering_of_pic_nums_idc == 2) {
                    sliceHeader.long_term_pic_num = ReadGolomb(bits);
                }
            }
            while (sliceHeader.reordering_of_pic_nums_idc != 3);
        }
    }

    if ((pps->weighted_pred_flag &&
        (sliceType == SLICE_TYPE_P || sliceType == SLICE_TYPE_SP)) ||
            (pps->weighted_bipred_idc == 1 && sliceType == SLICE_TYPE_B)) {
        sliceHeader.luma_log2_weight_denom = ReadGolomb(bits);

        if (sps->chroma_format_idc != 0) {
            sliceHeader.chroma_log2_weight_denom = ReadGolomb(bits);
        }

        for (uint32_t i = 0; i <= sliceHeader.num_ref_idx_l0_active_minus1; ++i) {
            uint32_t lumaWeightL0Flag = bits.ReadBit();
            if (lumaWeightL0Flag) {
                ReadGolomb(bits);
                ReadGolomb(bits);
            }
            if (sps->chroma_format_idc != 0) {
                uint32_t chromaWeightL0Flag = bits.ReadBit();
                if (chromaWeightL0Flag) {
                    for (uint32_t j = 0; j < 2; ++j) {
                        ReadGolomb(bits);
                        ReadGolomb(bits);
                    }
                }
            }
        }
        if (sliceHeader.slice_type % 5 == 1) {
            for (uint32_t i = 0; i < sliceHeader.num_ref_idx_l1_active_minus1; ++i) {
                uint32_t lumaWeightL1Flag = bits.ReadBit();
                if (lumaWeightL1Flag) {
                    ReadGolomb(bits);
                    ReadGolomb(bits);
                }
                if (sps->chroma_format_idc != 0) {
                    uint32_t chromaWeightL1Flag = bits.ReadBit();
                    if (chromaWeightL1Flag) {
                        for (uint32_t j = 0; j < 2; ++j) {
                            ReadGolomb(bits);
                            ReadGolomb(bits);
                        }
                    }
                }
            }
        }
    }

    if (naluRefIdc!= 0) {
        if (nalutype == NALU_TYPE_IDR) {
            sliceHeader.no_output_of_prior_pics_flag = bits.ReadBit();
            sliceHeader.long_term_reference_flag = bits.ReadBit();
        }
        else {
            uint32_t adaptiveRefPicMarkingModeFlag = bits.ReadBit();
            if (adaptiveRefPicMarkingModeFlag) {
                uint32_t memoryManagementControlOperation = 0;
                do {
                    memoryManagementControlOperation = ReadGolomb(bits);
                    if (memoryManagementControlOperation == 1 || memoryManagementControlOperation == 3) {
                        sliceHeader.difference_of_pic_nums_minus1 = ReadGolomb(bits);
                    }
                    if (memoryManagementControlOperation == 2) {
                        sliceHeader.long_term_pic_num = ReadGolomb(bits);
                    }
                    if (memoryManagementControlOperation == 3 || memoryManagementControlOperation == 6) {
                        sliceHeader.long_term_frame_idx = ReadGolomb(bits);
                    }
                    if (memoryManagementControlOperation == 4) {
                        sliceHeader.max_long_term_frame_idx_plus1 = ReadGolomb(bits);
                    }
                }
                while (memoryManagementControlOperation != 0);
            }
        }
    }

    if (pps->entropy_coding_mode_flag && sliceType != SLICE_TYPE_I && sliceType != SLICE_TYPE_SI) {
        sliceHeader.cabac_init_idc = ReadGolomb(bits);
    }
    sliceHeader.slice_qp_delta = ReadGolomb(bits);

    if (sliceType == SLICE_TYPE_SP || sliceType == SLICE_TYPE_SI) {
        if (sliceType == SLICE_TYPE_SP) {
            sliceHeader.sp_for_switch_flag = bits.ReadBit();
        }
        sliceHeader.slice_qs_delta = SignedGolomb(ReadGolomb(bits));
    }
    if (pps->deblocking_filter_control_present_flag) {
        sliceHeader.disable_deblocking_filter_idc = ReadGolomb(bits);

        if (sliceHeader.disable_deblocking_filter_idc != 1) {
            sliceHeader.slice_alpha_c0_offset_div2 = SignedGolomb(ReadGolomb(bits));
            sliceHeader.slice_beta_offset_div2 = SignedGolomb(ReadGolomb(bits));
        }
    }
    if (pps->num_slice_groups_minus1 > 0 &&
        pps->slice_group_map_type >= 3 &&
        pps->slice_group_map_type <= 5) {
        sliceHeader.slice_group_change_cycle = ReadGolomb(bits);
    }

    sliceHeader.size = bits.GetBitsRead();
    ln = bits.GetBitsRead();
    return true;
}


bool AvcFrameParser::SameFrame(uint32_t naltype1, uint32_t nalRefIdc1, AvcSliceHeader& header1,
               uint32_t naltype2, uint32_t nalRefIdc2, AvcSliceHeader& header2) {
    if (header1.frame_num != header2.frame_num) {
        return false;
    }

    if (header1.pic_parameter_set_id != header2.pic_parameter_set_id) {
        return false;
    }

    if (header1.field_pic_flag != header2.field_pic_flag) {
        return false;
    }
    if (header1.field_pic_flag) {
        if (header1.bottom_field_flag != header2.bottom_field_flag) {
            return false;
        }
    }

    if ((nalRefIdc1 == 0 || nalRefIdc2 == 0) && (nalRefIdc1 != nalRefIdc2)) {
        return false;
    }

    MuxSps* sps = GetSliceSps(header1);

    if (sps == nullptr) {
        return false;
    }

    if (sps->pic_order_cnt_type == 0) {
        if (header1.pic_order_cnt_lsb != header2.pic_order_cnt_lsb ||
            header1.delta_pic_order_cnt[0] != header2.delta_pic_order_cnt[0]) {
            return false;
        }
    }
    if (sps->pic_order_cnt_type == 1) {
        if (header1.delta_pic_order_cnt[0] != header2.delta_pic_order_cnt[0] ||
            header1.delta_pic_order_cnt[1] != header2.delta_pic_order_cnt[1]) {
            return false;
        }
    }
    if (naltype1 == NALU_TYPE_IDR ||
        naltype2 == NALU_TYPE_IDR) {
        if (naltype1 != naltype2) {
            return false;
        }

        if (header1.idr_pic_id != header2.idr_pic_id) {
            return false;
        }
    }
    return true;
}

AvcFrameParser::~AvcFrameParser() {
    for (int i = 0; i < 256; ++i) {
        delete spsVec_[i];
        delete ppsVec_[i];
    }
}

std::vector<Box *> AvcFrameParser::GetBox() const {
    return boxes_;
}

