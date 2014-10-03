#include "test.h"
#include "zerror.h"

int runTests(ZAssoc<ZString, test_func> tests);

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZPath lgf = ZPath("logs") + ZLog::genLogFileName("testchaos_");
    ZLog::addLogFile(lgf, ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::addLogFile(lgf, ZLogSource::debug, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZLog::addLogFile(lgf, ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");

    ZAssoc<ZString, test_func> defaulttests = {
        { "array", array_block },
        { "binary", binary_block },
        { "assoc", assoc_block },
        { "string", string_block },
        { "path", path_block },
        { "path_windows", path_windows_block },
        { "pointer", pointer_block },
        { "thread", thread_block },
        { "number", number_block },
        { "file", file_block },
        { "png", png_block },
        { "autobuffer", autobuffer_block },
        { "json", json_block },
    };

    ZAssoc<ZString, test_func> tests = defaulttests;
    tests.concat({
        { "udp", udp_test },
        { "udpserver", udpserver_test },
        { "tcp", tcp_test },
        { "tcpserver", tcpserver_test },
        { "tcpserver2", tcpserver_test2 },
        { "tcpserver3", tcpserver_test3 },

        { "sandbox", sandbox },

        { "error", error_block },
    });

    if(argc > 1){
        ZAssoc<ZString, test_func> runtests;
        ZString runstr;
        if(ZString(argv[1]) == "all"){
            runtests = defaulttests;
            runstr = "All";
        } else {
            for(int i = 1; i < argc; ++i){
                ZString key = argv[i];
                 if(tests.exists(key)){
                    runtests.push(key, tests[key]);
                    runstr << key << ",";
                } else {
                    LOG("No Test " << key << ", Ignoring");
                }
            }
        }
        runstr.strip(',');

        LOG("*** Starting Tests \"" << runstr << "\"");
        int ret = runTests(runtests);
        LOG("*** Finished Tests \"" << runstr << "\"");
        return ret;
    }
    return 0;
}

int runTests(ZAssoc<ZString, test_func> tests){
    for(zu64 i = 0; i < tests.size(); ++i){
        LOG("=== Starting " << tests.key(i) << " Test...");
        int result = -1;
        try {
            result = tests[i]();
        } catch(int err){
            result = err;
        } catch(ZError err){
            ELOG("!! Error: " << err.what());
            err.logStackTrace();
            result = -2;
        }
        if(result != 0){
            ELOG("!!! Test '" << tests.key(i) << "' Failed: " << result);
        } else {
            LOG("=== Finished Test '" << tests.key(i) << "'");
        }
    }
    return 0;
}
