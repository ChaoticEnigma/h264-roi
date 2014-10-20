#ifndef ZH264DECODER_H
#define ZH264DECODER_H

#include "zfile.h"
#include "zimage.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
}

namespace LibChaos {

class ZH264Decoder {
public:
    ZH264Decoder();

private:
    AVCodec *codec;                 // h264 decoder
    AVCodecContext *context;        // Decoder state
    AVCodecParserContext *parser;   // h264 bitstream parser
    AVFrame *frame;                 // Current decoded frame

    ZFile file;                     // File with h264 data

    zbyte *buffer;                  // Buffered file data
    zu64 buffersize;                // Size of current buffer
};

}

#endif // ZH264DECODER_H
