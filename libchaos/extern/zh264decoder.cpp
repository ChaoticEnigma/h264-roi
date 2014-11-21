#include "zh264decoder.h"
#include "zlog.h"

namespace LibChaos {

ZH264Decoder::ZH264Decoder() : codec(NULL), context(NULL), parser(NULL), frame(NULL), ok(false), callback(nullptr){
    avcodec_register_all();
}

ZH264Decoder::ZH264Decoder(ZPath path, decoderCallback callback, void *userptr) : ZH264Decoder(){
    open(path, callback, userptr);
}

ZH264Decoder::~ZH264Decoder(){
    if(parser) {
        av_parser_close(parser);
        parser = NULL;
    }

    if(context) {
        avcodec_close(context);
        av_free(context);
        context = NULL;
    }

    if(frame) {
        av_free(frame);
        frame = NULL;
    }

    callback = NULL;
    user = NULL;
}

//AVPixelFormat pickDecodeFormat(AVCodecContext *s, const AVPixelFormat *fmt){
//    return AV_PIX_FMT_YUV420P;
//}

bool ZH264Decoder::open(ZPath path, decoderCallback framecallback, void *userptr){
    ok = false;

    // Create AVCodec
    codec = avcodec_find_decoder(CODEC_ID_H264);
//    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(!codec){
        ELOG("Error: cannot find the H264 codec");
        return false;
    }

    // Alloc context
    context = avcodec_alloc_context3(codec);
    if(codec->capabilities & CODEC_CAP_TRUNCATED){
        context->flags |= CODEC_FLAG_TRUNCATED;
    }

    //context->get_format = pickDecodeFormat;

    // Verify
    if(avcodec_open2(context, codec, NULL) < 0){
        ELOG("Error: could not open codec");
        return false;
    }

    // Open file
    file.open(path);
    if(!file.isOpen()){
        ELOG("Error: could not open file");
        return false;
    }

    // Init parser
    frame = avcodec_alloc_frame();
//    frame = av_frame_alloc();
    parser = av_parser_init(CODEC_ID_H264);
    if(!parser) {
        ELOG("Erorr: cannot create H264 parser");
        return false;
    }

    framecount = 0;

    callback = framecallback;
    user = userptr;
    ok = true;
    return true;
}

double ZH264Decoder::getFPS(){
    if(!ok)
        return 0;

    AVRational rational = context->time_base;
    return av_q2d(rational);
}

bool ZH264Decoder::readFrame(){
    if(!ok)
        return false;

    if(buffer.isEmpty())
        readData();

    uint8_t *data = NULL;
    int size = 0;
    int len;
    while(true){
        len = av_parser_parse2(parser, context, &data, &size, buffer.raw(), buffer.size(), 0, 0, AV_NOPTS_VALUE);
        if(size == 0 && len >= 0){
            readData();
            continue;
        }
        break;
    }

    if(len){
//        decodeFrame(data, size);
        decodeFrame(buffer.raw(), size);
        buffer.erase(0, len);
        return true;
    }

    return false;
}

void ZH264Decoder::readData(){
    zu64 oldsize = buffer.size();
    buffer.resize(oldsize + ZH264_READ_CHUNK_SIZE);
    zu64 len = file.read(buffer.raw() + oldsize, ZH264_READ_CHUNK_SIZE);
    if(len < ZH264_READ_CHUNK_SIZE)
        buffer.resize(oldsize + len);
}

void ZH264Decoder::decodeFrame(zbyte *data, zu64 size){
    AVPacket packet;
    av_init_packet(&packet);

    packet.data = data;
    packet.size = size;

    int got_picture = 0;
    int len = avcodec_decode_video2(context, frame, &got_picture, &packet);
    if(len < 0){
        ELOG("Error while decoding a frame.\n");
    }

    if(got_picture == 0) {
        return;
    }

    ++framecount;

    if(callback) {
        callback(framecount, frame, &packet, this, user);
    }
}

}
