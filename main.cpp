#include "main.h"
#include "zbmp.h"
#include "zppm.h"
#include "zfile.h"
#include "zlist.h"

#include <string>

#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

struct Region {
    zu32 x1;
    zu32 y1;
    zu32 x2;
    zu32 y2;
    float offset;
};

struct UserData {
    ZH264Encoder *encoder;
    ZPath output;
    float baseqp;
    ZList<Region> regions;
};

bool brear = false;
bool setup = false;
bool cont = true;

//void decoderCallback(AVFrame *frame, AVPacket *pkt, const H264_Decoder *decode, void *user){
void decoderCallback(zu32 num, AVFrame *frame, AVPacket *pkt, const ZH264Decoder *decode, void *user){

    if(!setup && (decode->context->pix_fmt != AV_PIX_FMT_YUV420P || frame->format != AV_PIX_FMT_YUV420P)){
        LOG("Incorrect decode pixel format " << decode->context->pix_fmt << " " << frame->format);
    }

    if(user != nullptr){
        UserData *userdata = (UserData*)user;
        ZH264Encoder *encoder = userdata->encoder;
        if(!setup){
            // Setup encoder
            LOG(encoder->inputSetup(frame->width, frame->height, 0));
            LOG(encoder->outputSetup(frame->width, frame->height, 30));
            encoder->infmt = decode->context->pix_fmt;

            // Set up regions in macroblocks
            zu32 xblocks = frame->width / 16 + (frame->width % 16 ? 1 : 0);
            zu32 yblocks = frame->height / 16 + (frame->height % 16 ? 1 : 0);
            encoder->quantOffsets().resize(xblocks * yblocks);

            for(zu64 i = 0; i < xblocks * yblocks; ++i){
                encoder->quantOffsets()[i] = userdata->baseqp;
            }
            LOG("Setting " << userdata->regions.size() << " regions");
            for(zu64 i = 0; i < userdata->regions.size(); ++i){
                LOG(userdata->regions[i].x1 << "," << userdata->regions[i].y1 << "," << userdata->regions[i].x2 << "," << userdata->regions[i].y2 << ":" << userdata->regions[i].offset);
                for(zu32 y = userdata->regions[i].y1; y <= userdata->regions[i].y2 && y < yblocks; ++y){
                    for(zu32 x = userdata->regions[i].x1; x <= userdata->regions[i].x2 && x < xblocks; ++x){
                        encoder->quantOffsets()[x + y * xblocks] = userdata->regions[i].offset;
                    }
                }
            }

            if(!encoder->open(userdata->output)){
                throw ZError("Failed to open encoder");
            }
            LOG("Encoder Open");
            setup = true;
        }
        encoder->encode(frame->data, frame->linesize);
    } else {
        ELOG("Couldn't encode");
    }

    if(num % 10 == 0){
        LOG("Update: " << num);
    }

//    LOG("Read frame: " << frame->coded_picture_number);

//    if(frame->coded_picture_number == 300){
//        LOG("Read frame: "
//            << frame->width << "x" << frame->height << " "
//            << frame->coded_picture_number << " " << frame->display_picture_number << " " << pkt->stream_index << " "
//            << frame->linesize[0] << "," << frame->linesize[1] << "," << frame->linesize[2] << " "
//            //<< frame->channels
//        );

//        ZImage image;
//        image.convertYUV420toRGB24(frame->width, frame->height, frame->data[0], frame->data[1], frame->data[2]);

//        ZPPM ppm(image);
//        LOG(ppm.write("outppm.ppm"));

//        ZBMP bmp(image);
//        LOG(bmp.write("outbmp.bmp"));

//        if(brear)
//            cont = false;
//    }
}

void freeQuants(void *ptr){
    LOG("Free quantizer offsets");
    //delete[] (float*)ptr;
}

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    //ZLog::init();
    LOG("Starting H264-ROI");

    ArZ args;
    for(int i = 1; i < argc; ++i){
        args.push(argv[i]);
    }

    if(args.size() < 3){
        LOG("Usage: <input> <output> <qp_factor> [regions]...");
        return 1;
    }

    ZPath input = args[0];
    ZPath output = args[1];
    float baseqp = stof(args[2].str());
    ZList<Region> regions;
    for(zu64 i = 3; i < args.size(); ++i){
        ArZ tokens = args[i].split(':');
        if(tokens.size() != 2)
            return 2;
        ArZ coords = tokens[0].explode(',');
        if(coords.size() != 4)
            return 3;
        regions.push({ (zu32)coords[0].tozu64() / 16, (zu32)coords[1].tozu64() / 16, (zu32)coords[2].tozu64() / 16, (zu32)coords[3].tozu64() / 16, stof(tokens[1].str()) });
    }

    UserData userdata;
    ZH264Decoder decoder;
    if(!decoder.open(input, decoderCallback, &userdata)){
        ELOG("Failed to open decoder");
        return 4;
    }
    LOG("Decoder Open");

    ZH264Encoder encoder;
    userdata.output = output;
    userdata.encoder = &encoder;
    userdata.baseqp = baseqp;
    userdata.regions = regions;

    LOG("Reading frames...");

    brear = true;
    while(cont){
        decoder.readFrame();
//        if(!decoder.readFrame()){
//            LOG("Failed to decode frame");
//            break;
//        }
    }

    encoder.close();

    LOG("Finished H264-ROI");
    return 0;
}
