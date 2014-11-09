#include "test.h"
#include "zerror.h"

int runTests(ZAssoc<ZString, test_func> tests);

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::normal, "%clock% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%clock% %thread% %function% (%file%:%line%) - %log%");
    ZPath lgf = ZPath("logs") + ZLog::genLogFileName("testchaos_");
    ZLog::addLogFile(lgf, ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::addLogFile(lgf, ZLogSource::debug, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZLog::addLogFile(lgf, ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");

    ZAssoc<ZString, test_func> defaulttests = {
        { "pointer", pointer_block },
        { "allocator", allocator_block },
        { "storage", storage_block },

        { "binary", binary_block },

        { "array", array_block },
        { "assoc", assoc_block },
        { "list", list_block },

        { "string", string_block },
        { "path", path_block },

        { "thread", thread_block },
        { "number", number_block },

        { "file", file_block },
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

#ifdef TEST_ZPNG
        { "png", png_block },
#endif

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
    ArZ errorstrings;
    for(zu64 i = 0; i < tests.size(); ++i){
        LOG("=== Starting Test '" << tests.key(i) << "'...");
        int result = -1;
        try {
            result = tests[i]();
        } catch(int err){
            result = err;
        } catch(ZError err){
            ZString errstr = "!! Error: " + err.what();
            errorstrings.push(errstr);
            ELOG(errstr);
            err.logStackTrace();
            result = -2;
        }
        if(result != 0){
            ZString errstr = "!!! Test '" + tests.key(i) + "' Failed: " + result;
            errorstrings.push(errstr);
            ELOG(errstr);
        } else {
            LOG("=== Finished Test '" << tests.key(i) << "'");
        }
    }
    if(!errorstrings.isEmpty()){
        for(zu64 i = 0; i < errorstrings.size(); ++i){
            LOG("ERR: " << errorstrings[i]);
        }
    }
    return 0;
}
