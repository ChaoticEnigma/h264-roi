#include "main.h"

int main(){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    LOG("Starting H264-ROI");



    LOG("Finished H264-ROI");
    return 0;
}
