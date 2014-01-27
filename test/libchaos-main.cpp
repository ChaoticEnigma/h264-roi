#include "test.h"

int main(){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZPath lgf = ZString("logs") + ZLog::genLogFileName("testchaos_");
    ZLog::addLogFile(lgf, ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::addLogFile(lgf, ZLogSource::debug, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZLog::addLogFile(lgf, ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    LOG("Starting LibChaos Test");

    /*
    if(array_block() != 0) return 1;
    if(array_block2() != 0) return 2;

    if(string_block() != 0) return 3;
    if(string_magic_block() != 0) return 4;
    if(path_block() != 0) return 5;
    if(path_windows_block() != 0) return 6;

    if(file_block() != 0) return 7;

    if(thread_block() != 0) return 8;
    */

    if(autobuffer_block() != 0) return 9;

#if PLATFORM == LINUX
    // Always last
    if(error_block() != 0) return 10;
#endif
    LOG("LibChaos Test Done.");
    return 0;
}
