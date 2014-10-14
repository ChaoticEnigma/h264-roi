#include "main.h"
#include "zbmp.h"

#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "frame.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
}

void decoderCallback(AVFrame *frame, AVPacket *pkt, void *user){
    LOG("Read frame: " << frame->width << " " << frame->height << " " << frame->coded_picture_number << " " << frame->display_picture_number);
    if(frame->coded_picture_number == 283){
        ZImage image((zu64)frame->width, (zu64)frame->height);
        image.newData();
        memcpy(image.buffer(), frame->data[0], frame->linesize[0]);
        memcpy(image.buffer(), frame->data[1], frame->linesize[1]);
        memcpy(image.buffer(), frame->data[2], frame->linesize[2]);
        ZBMP bmp(image);
        LOG(bmp.write("out.bmp"));

        SaveFrame(frame, frame->width, frame->height, frame->coded_picture_number);
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

    while(true){
        decoder.readFrame();
    }

    //X264Encoder encoder;

    LOG("Finished H264-ROI");
    return 0;
}
