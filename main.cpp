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
    zu64 framecount;
};

bool brear = false;
bool setup = false;
bool cont = true;

void freeQuantOffsets(void *ptr){
    LOG("Free quantizer offsets");
    delete[] (float*)ptr;
}

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
            LOG("FPS: " << decode->getFPS());
            LOG("Input Setup: " << encoder->inputSetup(frame->width, frame->height, decode->getFPS()));
            LOG("Output Setup: " << encoder->outputSetup(frame->width, frame->height, decode->getFPS()));
            encoder->infmt = decode->context->pix_fmt;

            userdata->framecount = 0;

            // Set up regions in macroblocks
            zu32 xblocks = frame->width / 16 + (frame->width % 16 ? 1 : 0);
            zu32 yblocks = frame->height / 16 + (frame->height % 16 ? 1 : 0);
            encoder->quantOffsets().resize(xblocks * yblocks);

            for(zu64 i = 0; i < xblocks * yblocks; ++i){
                encoder->quantOffsets()[i] = userdata->baseqp;
            }
            LOG("Setting " << userdata->regions.size() << " regions:");
            for(zu64 i = 0; i < userdata->regions.size(); ++i){
                LOG(i << ": " << userdata->regions[i].x1 << "," << userdata->regions[i].y1 << "," << userdata->regions[i].x2 << "," << userdata->regions[i].y2 << "  qp: " << userdata->regions[i].offset);
                for(zu32 y = userdata->regions[i].y1; y <= userdata->regions[i].y2 && y < yblocks; ++y){
                    for(zu32 x = userdata->regions[i].x1; x <= userdata->regions[i].x2 && x < xblocks; ++x){
                        encoder->quantOffsets()[x + y * xblocks] = userdata->regions[i].offset;
                    }
                }
            }

            if(!encoder->open(userdata->output)){
                throw ZException("Failed to open encoder");
            }
            LOG("Encoder Opened: " << userdata->output);
            setup = true;
        }

        encoder->encode(frame->data, frame->linesize);

        //userdata->framecount = decode->getFrameCount();

        if(num % 10 == 0){
            RLOG("\r" << "Update: " << num << "/" << userdata->framecount);
        }
        if(num == userdata->framecount){
            cont = false;
        }

    } else {
        ELOG("Couldn't encode (user == nullptr)");
    }
}

int main(int argc, char **argv){
    ZLog::logLevelStdOut(ZLog::INFO, "%time% %thread% - %log%");
    ZLog::logLevelStdErr(ZLog::ERRORS, "%time% %thread% %function% (%file%:%line%) - %log%");
    //ZLog::init();
    LOG("Starting H264-ROI");

    ArZ args;
    for(int i = 1; i < argc; ++i){
        ZString arg = argv[i];
        arg.strip('"');
        args.push(arg);
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
        regions.push({ (zu32)coords[0].toUint() / 16,
                       (zu32)coords[1].toUint() / 16,
                       (zu32)coords[2].toUint() / 16,
                       (zu32)coords[3].toUint() / 16,
                       stof(tokens[1].str()) });
    }

    UserData userdata;
    ZH264Decoder decoder;
    if(!decoder.open(input, decoderCallback, &userdata)){
        ELOG("Failed to open decoder");
        return 4;
    }
    LOG("Decoder Opened: " << input);

    ZH264Encoder encoder;
    userdata.output = output;
    userdata.encoder = &encoder;
    userdata.baseqp = baseqp;
    userdata.regions = regions;

    LOG("Reading frames...");

    brear = true;

    while(cont){
        try {
        decoder.readFrame();
        } catch(ZException e){
            if(e.code() != 5)
                ELOG("Exception caught!");
            break;
        }
    }
    RLOG(ZLog::NEWLN);

    encoder.close();

    LOG("Finished H264-ROI");
    return 0;
}
