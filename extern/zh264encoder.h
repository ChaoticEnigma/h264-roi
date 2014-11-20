#ifndef ZH264ENCODER_H
#define ZH264ENCODER_H

#include "zpath.h"
#include "zfile.h"
#include "zimage.h"

extern "C" {
    #include <libswscale/swscale.h>
    #include <libavcodec/avcodec.h>
    #include <x264.h>
}

namespace LibChaos {

class ZH264Encoder {
public:
    ZH264Encoder();
    ~ZH264Encoder();

    bool inputSetup(zu64 width, zu64 height, zu32 fps);
    bool outputSetup(zu64 width, zu64 height, zu32 fps);
    bool validSettings();

    bool open(ZPath path);
    void close();

    bool ready() const { return encoder_ready; }

    bool encode(ZImage &image);

    // Takes YUV420P data
    bool encode(uint8_t *data[], const int linesize[]);

    ZArray<float> &quantOffsets(){ return blockqps; }

private:
    //bool updateScaling();

private:
    // Context data
    x264_t *encoder;            // x264 Encoder
    x264_param_t *param;        // x264 param
    SwsContext *sws;            // Sws scaling context
    ZFile file;                 // File to write h264 data
    bool encoder_ready;

    // Repeatedly used data
    x264_nal_t *nals;           // NAL array
    int nalcount;               // Number of NALs
    x264_picture_t inpicture;   // x264 picture input to x264_encoder_encode
    x264_picture_t outpicture;  // x264 picture output from x264_encoder_encode

    // Input settings
    zu64 inwidth, inheight;     // Input width and height
    zu32 infps;                 // Input FPS

    // Output settings
    zu64 outwidth, outheight;   // Output width and height
    zu32 outfps;                // Output FPS

    // Macroblock setings
    ZArray<float> blockqps;
};

}

#endif // ZH264ENCODER_H
