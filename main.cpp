#include "main.h"
#include "zbmp.h"
#include "zppm.h"
#include "zfile.h"

#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

bool brear = false;
bool cont = true;

void saveFrame(AVFrame *pFrame, int width, int height){
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "frame.ppm");
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y * pFrame->linesize[0], 1, width * 3, pFile);

    // Close file
    fclose(pFile);
}

void decoderCallback(AVFrame *frame, AVPacket *pkt, const H264_Decoder *decode, void *user){
    AVPixelFormat fmt = decode->codec_context->pix_fmt;
    if(fmt == AV_PIX_FMT_YUV420P){
        if(user != nullptr){
            X264Encoder *encoder = (X264Encoder*)user;
            encoder->encode(frame->data, frame->linesize);
        }
        //LOG("Read frame: " << frame->coded_picture_number);

        if(frame->coded_picture_number == 350){
            LOG("Read frame: "
                << frame->width << "x" << frame->height << " "
                << frame->coded_picture_number << " " << frame->display_picture_number << " " << pkt->stream_index << " "
                << frame->linesize[0] << "," << frame->linesize[1] << "," << frame->linesize[2] << " "
                << frame->channels
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

int main(){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZLog::init();
    LOG("Starting H264-ROI");

    X264Encoder encoder;

    H264_Decoder decoder(decoderCallback, &encoder);

#if PLATFORM == LINUX
    //LOG("Load: " << decoder.load("/home/chaos/Shared/bf4 hs2.mp4"));
    LOG("Load: " << decoder.load("/home/chaos/Shared/sao1.h264"));
#else
    //LOG("Load: " << decoder.load("F:\\Video\\bf4 hs2.mp4"));
    //LOG("Load: " << decoder.load("E:\\Users\\Chaos\\Shared\\sao1.h264"));
    LOG("Load: " << decoder.load("sao1.h264"));
    //LOG("Load: " << decoder.load(rx_to_data_path("sao1.h264"), 30.0f));
#endif

    encoder.in_width = 1280;
    encoder.in_height = 720;
    encoder.in_pixel_format = AV_PIX_FMT_YUV420P;
    encoder.fps = 24;
    encoder.out_width = 1280;
    encoder.out_height = 720;
    encoder.out_pixel_format = AV_PIX_FMT_YUV420P;

    zu32 xblocks, yblocks;
    xblocks = encoder.out_width / 16;
    if(encoder.out_width % 16) ++xblocks;
    yblocks = encoder.out_height / 16;
    if(encoder.out_height % 16) ++yblocks;

    float *quants = new float[xblocks * yblocks];

    for(zu64 x = 0; x < xblocks; ++x){
        for(zu64 y = 0; y < yblocks; ++y){
            if(x < xblocks / 2)
                quants[x + y * xblocks] = 20;
            else
                quants[x + y * xblocks] = 60;
        }
    }

    LOG("Blocks: " << xblocks << " " << yblocks);

    LOG("Open: " << encoder.open("sao1-2.h264", false));

    encoder.pic_in.prop.quant_offsets = quants;

    LOG("Reading frames...");

    brear = true;
    while(cont){
        decoder.readFrame();
    }

    encoder.close();

    cont = true;
    brear = true;
    H264_Decoder decoder2(decoderCallback, nullptr);
    LOG(decoder2.load("sao1-2.h264"));
    while(cont){
        decoder2.readFrame();
    }

    LOG("Finished H264-ROI");
    return 0;
}
