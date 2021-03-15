#include <vector>
#include <cstring>
#include <algorithm>
#include <thread>
#include <future>
#include <cassert>
#include <fstream>

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

static void DoCalculateShortTimeEnergy(uint8_t* data,
                                           long ndata,
                                           long samplePerFrame,
                                           int sampleWidth,
                                           int nchannel,
                                           int64_t filterVal,
                                           int64_t timeBase,
                                           int64_t frameSize,
                                           std::promise<std::vector<int64_t>>& retVal) {
                                            
    long nsample = ndata / sampleWidth; 
    std::vector<int64_t> timeStample;
    int64_t sum = 0;
    bool energyAppear = false;
    int64_t currentTime = timeBase;

    int step = sampleWidth*nchannel;

    int i = 0;
    int sampleCnt = 0;
    while (i < ndata && i+step <= ndata) {
        int16_t sample = 0;
        sample |= data[i];
        sample |= data[i+1] << 8;
        sum = sum + (sample * sample);
        if ((sampleCnt+1) % samplePerFrame == 0) {
            if (sum < filterVal) {
                sum = 0;
                energyAppear = false;
            }
            if (sum > 0 && !energyAppear) {
                timeStample.push_back(currentTime);
                energyAppear = true;

                // by pass 2000ms

                int byPassSamples = (2000/frameSize)*samplePerFrame;
                sampleCnt += byPassSamples;
                i += byPassSamples * sampleWidth;
                currentTime += 2000;
                continue;
            }
            sum = 0;
            currentTime += frameSize;
        }
        else if (sampleCnt == nsample-1) {
            if (sum < filterVal) {
                sum = 0;
            }
            if (sum > 0 && !energyAppear) {
                timeStample.push_back(currentTime);
            }

        }
        

        i += step;
        ++sampleCnt;
    }

    retVal.set_value(timeStample);
}


static void Write(std::ofstream& outStream, const std::vector<int64_t>& data) {
    for (int64_t val : data) {
        outStream << val << std::endl;
    }
}


static AVFormatContext* OpenVideoStream(const char* path) {
    AVFormatContext* ctx{nullptr};
    if (avformat_open_input(&ctx, path, nullptr, nullptr) < 0) {
        ::printf("open file failed\n");
        avformat_close_input(&ctx);
        return nullptr;
    }
    if (avformat_find_stream_info(ctx, nullptr) < 0) {
        ::printf("find stream info failed\n");
        avformat_close_input(&ctx);
        return nullptr;
    }
    if (ctx->nb_streams < 2) {
        ::printf("video format error\n");
        avformat_close_input(&ctx);
        return nullptr;
    }
    return ctx; 
}


static int GetAudioStreamPos(AVFormatContext* inCtx) {
    int audioStreamIndex = av_find_best_stream(inCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audioStreamIndex < 0) {
        ::printf("av_find_best_stream failed，check input file\n");
        avformat_close_input(&inCtx);
        return -1;
    }
    return audioStreamIndex;
}

static AVCodec* GetCodec(AVCodecID codecId, AVFormatContext* ctx) {
    AVCodec* codec = avcodec_find_decoder(codecId);
    if (codec == nullptr) {
        ::printf("can not find codec\n");
        avformat_close_input(&ctx);
        return nullptr;
    }
    return codec;
}

static bool OpenCodec(AVCodecContext* codecCtx, AVCodec* codec, AVFormatContext* ctx) {
    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        ::printf("open codec failed\n");
        avformat_close_input(&ctx);
        return false;
    }
    return true;
}

static std::vector<uint8_t> AacToPcm(AVFormatContext* inCtx, AVCodecContext* codecCtx, int audioIndex) {
    std::vector<uint8_t> res;
    const int MAX_AUDIO_FRAME_SIZE = 192000;
    uint64_t channelLayout = codecCtx->channels == 1 ? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
    AVSampleFormat sampleFmt = AV_SAMPLE_FMT_S16;
    int nbSamples = codecCtx->frame_size;
    int sampleRate = codecCtx->sample_rate;
    int nchannels = codecCtx->channels;
    int bufferSize = av_samples_get_buffer_size(nullptr, nchannels, nbSamples, sampleFmt, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(MAX_AUDIO_FRAME_SIZE);


    SwrContext* swrCtx = swr_alloc();
    swr_alloc_set_opts(swrCtx, channelLayout, sampleFmt, sampleRate, codecCtx->channel_layout, codecCtx->sample_fmt, codecCtx->sample_rate, 0, nullptr);;
    swr_init(swrCtx);
    AVPacket pkt;
    AVFrame* frame = av_frame_alloc();

    while (av_read_frame(inCtx, &pkt) >= 0) {
        int got = -1;
        if (pkt.stream_index == audioIndex) {
            if (avcodec_decode_audio4(codecCtx, frame, &got, &pkt) < 0) {
                ::printf("decoding audio frame error\n");
                av_free(frame);
                av_free(buffer);
                swr_free(&swrCtx);
                return std::vector<uint8_t>();
            }
            if (got != -1) {
                swr_convert(swrCtx, &buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t**)frame->data, frame->nb_samples);
                int beg = res.size();
                res.resize(res.size()+bufferSize);
                std::copy(buffer, buffer+bufferSize, res.data()+beg);
            }
        }
    }
    av_free(frame);
    av_free(buffer);
    swr_free(&swrCtx);
    return res;
}

static void CalculateShortTimeEnergyInMultithread(std::vector<uint8_t>& audioData,
                                                       AVStream* audioStream,
                                                       int frameSize,
                                                       int64_t filterVal,
                                                       int threadNum
                                                       ) {
    using namespace std;                                                        
    // audioData 也许包含多通道数据
    long nchannel = audioStream->codec->channels;
    long sampleRate = audioStream->codec->sample_rate;
    long sampleWidth = audioStream->codec->bits_per_coded_sample / 8;
    // nsample per channel
    long nsample = audioData.size() / sampleWidth / nchannel;
    long audioDuration = nsample * 1000 / sampleRate;
    long nframe = (int)(audioDuration / frameSize + 0.5);
    long samplePerFrame = nsample / nframe;
    long framePerThread = nframe / threadNum;


    assert(nsample >= threadNum);
    long ndataPerThread = nsample / threadNum * sampleWidth;
    uint8_t* data = audioData.data();

    vector<promise<vector<int64_t>>> promises(threadNum);
    vector<future<vector<int64_t>>> futures;
    for (int i = 0; i < threadNum; ++i) {
        futures.push_back(promises[i].get_future());
    }

    vector<uint8_t*> dataOffsets;
    for (int i = 0; i < threadNum; ++i) {
        dataOffsets.push_back(data + i*ndataPerThread);
    }

    vector<int64_t> timeOffsets;
    for (int i = 0; i < threadNum; ++i) {
        timeOffsets.push_back(i*framePerThread*frameSize);
    }

    vector<thread> threads;

    for (int i = 0; i < threadNum; ++i) {
        if (i == threadNum-1) {
            long ndata = (long)(audioData.size()-i*ndataPerThread);
            threads.push_back(thread(DoCalculateShortTimeEnergy,
                                     dataOffsets[i],
                                     ndata,
                                     samplePerFrame,
                                     sampleWidth,
                                     nchannel,
                                     filterVal,
                                     timeOffsets[i],
                                     frameSize,
                                     ref(promises[i])
                                     ));
        }
        else {
            threads.push_back(thread(DoCalculateShortTimeEnergy, 
                                     dataOffsets[i],
                                     ndataPerThread,
                                     samplePerFrame,
                                     sampleWidth,
                                     nchannel,
                                     filterVal,
                                     timeOffsets[i],
                                     frameSize,
                                     ref(promises[i])));
        }
    }

    for (int i = 0; i < threadNum; ++i) {
        threads[i].join();
    }



    std::ofstream out("./time_stample", std::ios::out | std::ios::trunc);
    for (int i = 0; i < threadNum; ++i) {
        Write(out, futures[i].get());
    }
}

bool CalculateShortTimeEnergy(const char* path, int64_t frameSize, const char* filterValStr, int threadNum) {
    av_register_all();

    int64_t filterVal = std::stoll(filterValStr);
    AVFormatContext* inFmtCtx = OpenVideoStream(path);
    if (inFmtCtx == nullptr) { return false; }

    int audioStreamIndex = GetAudioStreamPos(inFmtCtx);
    if (audioStreamIndex == -1) { return false; }

    // 音频流
    AVStream* inStream = inFmtCtx->streams[audioStreamIndex];

    // get codec 
    AVCodecContext* codecCtx = inStream->codec;
    AVCodec* codec = GetCodec(codecCtx->codec_id, inFmtCtx);
    if (codec == nullptr) { return false; }



    // open codec
    if (!OpenCodec(codecCtx, codec, inFmtCtx)) { return false; }


    std::vector<uint8_t> audioData = AacToPcm(inFmtCtx, codecCtx, audioStreamIndex);
    // AVPacket pkt;

    CalculateShortTimeEnergyInMultithread(audioData, inStream, frameSize, filterVal, threadNum);

    return true;
}
}
