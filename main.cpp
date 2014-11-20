#include "main.h"
#include "zbmp.h"
#include "zppm.h"
#include "zfile.h"

#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

bool brear = false;
bool cont = true;

void decoderCallback(AVFrame *frame, AVPacket *pkt, const H264_Decoder *decode, void *user){
    PixelFormat fmt = decode->codec_context->pix_fmt;
    if(fmt == PIX_FMT_YUV420P){
        if(user != nullptr){
            ZH264Encoder *encoder = (ZH264Encoder*)user;
            encoder->encode(frame->data, frame->linesize);
        }
        //LOG("Read frame: " << frame->coded_picture_number);

        if(frame->coded_picture_number == 300){
            LOG("Read frame: "
                << frame->width << "x" << frame->height << " "
                << frame->coded_picture_number << " " << frame->display_picture_number << " " << pkt->stream_index << " "
                << frame->linesize[0] << "," << frame->linesize[1] << "," << frame->linesize[2] << " "
                //<< frame->channels
            );

            ZImage image;
            image.convertYUV420toRGB24(frame->width, frame->height, frame->data[0], frame->data[1], frame->data[2]);

            ZPPM ppm(image);
            LOG(ppm.write("outppm.ppm"));

            ZBMP bmp(image);
            LOG(bmp.write("outbmp.bmp"));

            // ////////////////////////////////////////////

//            AVFrame *rgbframe;
//            rgbframe = av_frame_alloc();
//            avpicture_alloc((AVPicture*)rgbframe, AV_PIX_FMT_RGB24, frame->width, frame->height);

//            SwsContext *sws = sws_getContext(frame->width, frame->height, fmt, frame->width, frame->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
//            LOG(sws_scale(sws, frame->data, frame->linesize, 0, frame->height, rgbframe->data, rgbframe->linesize));
//            LOG(rgbframe->linesize[0]);

//            ZImage image2;
//            image2.setDimensions(frame->width, frame->height, 3, 8);
//            image2.copyData(rgbframe->data[0]);

//            ZPPM ppm2(image2);
//            LOG(ppm2.write("outppm2.ppm"));

            if(brear)
                cont = false;
        }
    }

    if(frame->coded_picture_number == 34118)
        cont = false;
}

void freeQuants(void *ptr){
    LOG("Free quantizer offsets");
    //delete[] (float*)ptr;
}

int main(){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    //ZLog::init();
    LOG("Starting H264-ROI");

    ZH264Encoder encoder;

    H264_Decoder decoder(decoderCallback, &encoder);

    LOG("Load: " << decoder.load("video.h264"));

    LOG(encoder.inputSetup(1280, 720, 0));
    LOG(encoder.outputSetup(1280, 720, 24));

    zu32 xblocks = 1280 / 16 + (1280 % 16 ? 1 : 0);
    zu32 yblocks = 720 / 16 + (720 % 16 ? 1 : 0);
    //float *quants = new float[xblocks * yblocks];
    encoder.quantOffsets().resize(xblocks * yblocks);

    for(zu64 i = 0; i < xblocks * yblocks; ++i){
        encoder.quantOffsets()[i] = 20.0f;
    }

    for(zu64 y = yblocks / 4; y < yblocks * 3 / 4; ++y){
        for(zu64 x = xblocks / 4; x < xblocks * 3 / 4; ++x){
            encoder.quantOffsets()[x + y * xblocks] = 0.0f;
        }
    }

    LOG("Open: " << encoder.open("lol-out.h264"));
    LOG("Reading frames...");

    brear = true;
    while(cont){
        decoder.readFrame();
    }

    encoder.close();

    LOG("Finished H264-ROI");
    return 0;
}
