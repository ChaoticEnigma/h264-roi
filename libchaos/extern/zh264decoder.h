#ifndef ZH264DECODER_H
#define ZH264DECODER_H

#include "zfile.h"
#include "zimage.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
}

//typedef int AVPixelFormat;
//typedef PixelFormat AVPixelFormat;

//enum {
//    AV_PIX_FMT_YUV420P = PIX_FMT_YUV420P,
//    AV_CODEC_ID_H264 = CODEC_ID_H264
//};

#define ZH264_READ_CHUNK_SIZE 16384

namespace LibChaos {

class ZH264Decoder {
public:
    typedef void (*decoderCallback)(zu32 number, AVFrame* frame, AVPacket* pkt, const ZH264Decoder *decodeFrame, void* user);

public:
    ZH264Decoder();
    ZH264Decoder(ZPath file, decoderCallback callback, void *userptr);
    ~ZH264Decoder();

    bool open(ZPath path, decoderCallback framecallback, void *userptr);
    bool isOk() const { return ok; }

    double getFPS();

    bool readFrame();

private:
    void readData();
    void decodeFrame(zbyte *data, zu64 size);

public:
    AVCodec *codec;                 // h264 decoder
    AVCodecContext *context;        // Decoder state
    AVCodecParserContext *parser;   // h264 bitstream parser
    AVFrame *frame;                 // Current decoded frame
    zu32 framecount;

    ZFile file;                     // File with h264 data

    bool ok;

    ZArray<zbyte> buffer;           // Buffered file data

    decoderCallback callback;
    void *user;
};

}

#endif // ZH264DECODER_H
