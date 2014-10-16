#include "main.h"
#include "zbmp.h"
#include "zppm.h"
#include "zfile.h"

#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

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

void decoderCallback(AVFrame *frame, AVPacket *pkt, void *user){
    AVPixelFormat fmt = ((H264_Decoder*)user)->codec_context->pix_fmt;
    if(fmt == AV_PIX_FMT_YUV420P){
        if(frame->coded_picture_number == 811){
            LOG("Read frame: "
                << frame->width << "x" << frame->height << " "
                << frame->coded_picture_number << " " << frame->display_picture_number << " " << pkt->stream_index << " "
                << frame->linesize[0] << "," << frame->linesize[1] << "," << frame->linesize[2] << " "
                << frame->channels
            );

            // ////////////////////////////////////////////

            //saveFrame(rgbframe, frame->width, frame->height);

//            ZBinary bin;
//            bin.concat(ZBinary(frame->data[0], frame->linesize[0] * frame->height));
//            bin.concat(ZBinary(frame->data[1], frame->linesize[1] * frame->height / 2));
//            bin.concat(ZBinary(frame->data[2], frame->linesize[2] * frame->height / 2));
//            LOG(ZFile::writeBinary("yuv", bin));

            ZImage image;
            image.convertYUV420toRGB24(frame->height, frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);

            ZPPM ppm(image);
            LOG(ppm.write("outppm.ppm"));

            ZBMP bmp(image);
            LOG(bmp.write("outbmp.bmp"));

            // ////////////////////////////////////////////

            AVFrame *rgbframe;
            rgbframe = av_frame_alloc();
            avpicture_alloc((AVPicture*)rgbframe, AV_PIX_FMT_RGB24, frame->width, frame->height);

//            uint8_t *dst_data[4];
//            int dst_linesize[4];
//            av_image_alloc(dst_data, dst_linesize, frame->width, frame->height, AV_PIX_FMT_RGB24, 16);

            SwsContext *sws = sws_getContext(frame->width, frame->height, fmt, frame->width, frame->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
            LOG(sws_scale(sws, frame->data, frame->linesize, 0, frame->height, rgbframe->data, rgbframe->linesize));
            LOG(rgbframe->linesize[0]);

            ZImage image2;
            image2.setDimensions(frame->width, frame->height, 3, 8);
            image2.copyData(rgbframe->data[0]);

            ZPPM ppm2(image2);
            LOG(ppm2.write("outppm2.ppm"));
        }
    }
}

int main(){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    LOG("Starting H264-ROI");

    H264_Decoder decoder(decoderCallback, &decoder);

#if PLATFORM == LINUX
    //LOG("Load: " << decoder.load("/home/chaos/Shared/bf4 hs2.mp4"));
    LOG("Load: " << decoder.load("/home/chaos/Shared/sao1.h264"));
#else
    //LOG("Load: " << decoder.load("F:\\Video\\bf4 hs2.mp4"));
    LOG("Load: " << decoder.load("E:\\Users\\Chaos\\Shared\\sao1.h264"));
    //LOG("Load: " << decoder.load(rx_to_data_path("sao1.h264"), 30.0f));
#endif

    LOG("Reading frames...");

    while(decoder.readFrame()){

    }

    //X264Encoder encoder;

    LOG("Finished H264-ROI");
    return 0;
}
