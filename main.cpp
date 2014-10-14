#include "main.h"

void *decoderCallback(AVFrame *frame, AVPacket *pkt, void *user){
    LOG("Read frame");
    return NULL;
}

int main(){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    LOG("Starting H264-ROI");

    H264_Decoder decoder((h264_decoder_callback)decoderCallback, NULL);

#if PLATFORM == LINUX
    //decoder.load("/home/chaos/Shared/bf4 hs2.mp4");
    decoder.load("/home/chaos/Shared/sao1.h264");
#else
    //decoder.load("F:\\Video\\bf4 hs2.mp4");
    decoder.load("E:\\Users\\Chaos\\Shared\\sao1.h264");
#endif

    LOG("Reading frames...");

    bool ok = true;
    while(ok){
        ok = decoder.readFrame();
        LOG(ok);
    }

    //X264Encoder encoder;

    LOG("Finished H264-ROI");
    return 0;
}
