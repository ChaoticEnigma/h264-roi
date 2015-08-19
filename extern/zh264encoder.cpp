#include "zh264encoder.h"
#include "zlog.h"

extern "C" {
    #include <libavutil/pixfmt.h>
}

namespace LibChaos {

ZH264Encoder::ZH264Encoder() : encoder(NULL), param(nullptr), sws(NULL), encoder_ready(false), nals(NULL), nalcount(0),
    inwidth(0), inheight(0), infps(0), infmt(PIX_FMT_YUV420P),
    outwidth(0), outheight(0), outfps(0){

}

ZH264Encoder::~ZH264Encoder(){
    close();
    delete param;
}

bool ZH264Encoder::inputSetup(zu64 width, zu64 height, zu32 fps){
    inwidth = width;
    inheight = height;
    infps = fps;

    if(!validSettings())
        return false;

    return true;
}

bool ZH264Encoder::outputSetup(zu64 width, zu64 height, zu32 fps){
    outwidth = width;
    outheight = height;
    outfps = fps;

    if(!validSettings())
        return false;

    return true;
}

bool ZH264Encoder::validSettings(){
    if(inwidth > 0 && inheight > 0 &&
       outwidth > 0 && outheight > 0 && outfps > 0)
        return true;
    return false;
}

bool ZH264Encoder::open(ZPath path){
    if(!validSettings()){
        ELOG("Encoder settings are invalid");
        return false;
    }

    if(ready()){
        ELOG("Encoder is already ready");
        return false;
    }

    if(!file.open(path, ZFile::WRITE)){
        ELOG("Cannot open output file");
        return false;
    }

    // setup x264 param
    delete param;
    param = new x264_param_t;
    int ret = x264_param_default_preset(param, "fast", "zerolatency");
    if(ret < 0)
        return false;

    param->i_threads = 1;
    param->i_width = outwidth;
    param->i_height = outheight;
    param->i_fps_num = outfps;
    param->i_fps_den = 1;

    // alloc picture buffer
    x264_picture_alloc(&inpicture, X264_CSP_I420, outwidth, outheight);
    inpicture.param = param;

    // set block qp offsets
    if(blockqps.size()){
        param->rc.i_aq_mode = X264_AQ_VARIANCE;
        inpicture.prop.quant_offsets = (float*)blockqps.ptr();
    }

    // get the scaling context
    sws = sws_getContext(inwidth, inheight, infmt, outwidth, outheight, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    //sws = sws_getContext(inwidth, inheight, AV_PIX_FMT_YUV420P, outwidth, outheight, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if(!sws){
        ELOG("Cannot create SWS context");
        return false;
    }

    // Open the encoder
    encoder = x264_encoder_open(param);
    if(!encoder) {
        ELOG("Cannot open the encoder");
        return false;
    }

    // Write headers to file
    int header_size = x264_encoder_headers(encoder, &nals, &nalcount);
    if(header_size < 0) {
        ELOG("x264_encoder_headers() failed");
        return false;
    }
    //int header_size = nals[0].i_payload + nals[1].i_payload + nals[2].i_payload;
    if(header_size){
        if(!file.write(nals[0].p_payload, (zu64)header_size)){
            ELOG("Cannot write headers");
            return false;
        }
    }

    encoder_ready = true;

    return true;
}

void ZH264Encoder::close(){
    encoder_ready = false;

    if(encoder) {
        x264_picture_clean(&inpicture);
        memset((char*)&inpicture, 0, sizeof(inpicture));
        memset((char*)&outpicture, 0, sizeof(outpicture));

        x264_encoder_close(encoder);
        encoder = NULL;
    }

    if(sws) {
        sws_freeContext(sws);
        sws = NULL;
    }

    file.close();
}

bool ZH264Encoder::encode(ZImage &image){

    return false;
}

bool ZH264Encoder::encode(uint8_t *data[], const int linesize[]){
    if(!validSettings()){
        ELOG("Encoder settings are invalid");
        return false;
    }
    if(!ready()){
        ELOG("Encoder is not ready");
        return false;
    }

    int scaleheight = sws_scale(sws, data, linesize, 0, inheight, inpicture.img.plane, inpicture.img.i_stride);
    if((zu64)scaleheight != outheight) {
        ELOG("scale failed: " << scaleheight);
        return false;
    }

    int frame_size = x264_encoder_encode(encoder, &nals, &nalcount, &inpicture, &outpicture);
    if(frame_size < 0){
        ELOG("Error while encoding");
        return false;
    }
    if(frame_size){
        if(!file.write(nals[0].p_payload, (zu64)frame_size)){
            ELOG("Error while trying to write nal");
            return false;
        }
    }

    return true;
}

//bool ZH264Encoder::updateScaling(){
//    if(!validSettings()){
//        ELOG("Encoder settings are invalid");
//        return false;
//    }

//    if(sws) {
//        sws_freeContext(sws);
//        sws = NULL;
//    }
//    sws = sws_getContext(inwidth, inheight, AV_PIX_FMT_YUV420P, outwidth, outheight, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
//    if(!sws){
//        ELOG("Cannot create SWS context");
//        return false;
//    }
//    return true;
//}

}
