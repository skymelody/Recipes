#include "video_module/ffmpeg_video_converter_impl.h"

#include <cassert>

#include <iostream>
extern "C" {
#include "libavformat/avformat.h"
}

using namespace std;

class AVFormatContextRAII {
public:
    AVFormatContextRAII(AVFormatContext* ctx): _ctx(ctx) {}
    AVFormatContextRAII(const char* filePath): _ctx(nullptr) {
        if (avformat_open_input(&_ctx, filePath, 0, 0) < 0) {
            _ctx = nullptr;
        }
        if (_ctx == nullptr || avformat_find_stream_info(_ctx, 0) < 0) {
            _ctx = nullptr;
        }
    }
    ~AVFormatContextRAII() {
        if (_ctx != nullptr) {
            avformat_close_input(&_ctx);
            _ctx = nullptr;
        }
    }

    AVFormatContext* Get() {
        return _ctx;
    }
private:
    AVFormatContext* _ctx;
};


bool FFMpegVideoConverterImpl::Convert(Video &inputVideo, Video& outputVideo){
    std::string inputPath = inputVideo.GetFilePath();
    std::string outputPath = outputVideo.GetFilePath();
    if (inputPath.empty() || outputPath.empty()) {
        return false;
    }

    av_register_all();

    /* 
        通过RAII手法管理对象，这样就可以很轻松的处理异常状况。
        发生异常后AVFromatContext流在返回时会自动关闭。
    */



    AVFormatContextRAII inFormatCtxRAII(inputPath.c_str());
    AVFormatContext* inFormatCtx = inFormatCtxRAII.Get();
    if (inFormatCtx == nullptr) {
        return false;
    }

    AVFormatContext* outFormatCtx = nullptr;
    avformat_alloc_output_context2(&outFormatCtx, nullptr, nullptr, outputPath.c_str());
    if (outFormatCtx == nullptr) {
        return false;
    }

    AVFormatContextRAII outCtxRAII(outFormatCtx);


    AVOutputFormat* outFormat = outFormatCtx->oformat;
    for (int i = 0; i < inFormatCtx->nb_streams; ++i) {
        AVStream* inStream = inFormatCtx->streams[i];
        AVCodec* codec = avcodec_find_decoder(inStream->codecpar->codec_id);
        AVStream* outStream = avformat_new_stream(outFormatCtx, codec);

        if (outStream == nullptr || inStream == nullptr || codec == nullptr) {
            return false;
        }

        AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(codecCtx, inStream->codecpar) < 0) {
            return false;
        }

        codecCtx->codec_tag = 0;
        if (outFormat->flags & AVFMT_GLOBALHEADER) {
            codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (avcodec_parameters_from_context(outStream->codecpar, codecCtx) < 0) {
            return false;
        }
    }

    if (!(outFormat->flags & AVFMT_NOFILE)) {
        if (avio_open(&outFormatCtx->pb, outputPath.c_str(), AVIO_FLAG_WRITE) < 0) {
            return false;
        }
    }

    if (avformat_write_header(outFormatCtx, nullptr) < 0) {
        return false;
    }

    while (true) {
        AVPacket packet;
        if (av_read_frame(inFormatCtx, &packet) < 0) {
            break;
        }

        AVStream* inStream = inFormatCtx->streams[packet.stream_index];
        AVStream* outStream = outFormatCtx->streams[packet.stream_index];

        packet.pts = av_rescale_q_rnd
        (
            packet.pts, 
            inStream->time_base, 
            outStream->time_base, 
            (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
        );
        packet.dts = av_rescale_q_rnd
        (
            packet.dts,
            inStream->time_base, 
            outStream->time_base, 
            (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX)
        );
        packet.duration = av_rescale_q
        (
            packet.duration, 
            inStream->time_base,
            outStream->time_base
        ); 
        packet.pos = -1;

        if (av_interleaved_write_frame(outFormatCtx, &packet) < 0) {
            return false;
        }

        av_packet_unref(&packet);
    }

    if (av_write_trailer(outFormatCtx) < 0) {
        return false;
    }

    return true;
}
