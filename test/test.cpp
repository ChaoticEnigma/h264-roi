#include "test.h"
#include "zexception.h"

#include <stdio.h>

int runTests(ZAssoc<ZString, test_func> tests);

int main(int argc, char **argv){
    try {
        //ZLog::init(); // BUG: threaded zlog sometimes crashes
        ZLog::formatStdout(ZLogSource::normal, "%clock% %thread% - %log%");
        ZLog::formatStdout(ZLogSource::debug, "%clock% %thread% - %log%");
        ZLog::formatStderr(ZLogSource::error, "%clock% %thread% %function% (%file%:%line%) - %log%");
        ZPath lgf = ZPath("logs") + ZLog::genLogFileName("testchaos_");
        ZLog::addLogFile(lgf, ZLogSource::normal, "%time% %thread% - %log%");
        ZLog::addLogFile(lgf, ZLogSource::debug, "%time% %thread% %function% (%file%:%line%) - %log%");
        ZLog::addLogFile(lgf, ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");

        LOG("Testing LibChaos: " << LibChaosDescribe());

        ZAssoc<ZString, test_func> defaulttests = {
            { "allocator", allocator_test },
            { "pointer", pointer_test },
            { "storage", storage_test },

            { "binary", binary_test },

            { "array", array_test },
            { "assoc", assoc_test },
            { "stack", stack_test },
            { "map", map_test },
            { "list", list_test },
            { "queue", queue_test },

            { "string", string_test },
            { "path", path_test },
            { "json", json_test },

            { "thread", thread_test },
            { "mutex", mutex_test },

            { "number", number_test },
            { "uid", uid_test },
            { "hash", hash_test },

            { "file", file_test },
            { "pdf", pdf_test },

#ifdef TEST_PNG
            { "image", image_test },
#endif
#ifdef TEST_PNG
            { "png", png_test },
#endif
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

            { "error", error_test },
        });

        ZAssoc<ZString, test_func> runtests;
        ZString runstr;
        if(argc > 1){
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
        } else {
            runtests = defaulttests;
            runstr = "All";
        }

        LOG("*** Starting Tests \"" << runstr << "\"");
        int ret = runTests(runtests);
        LOG("*** Finished Tests \"" << runstr << "\"");
        return ret;
    } catch(ZException e){
        printf("Catastrophic Failure: %s - %d\n%s\n", e.what().cc(), e.code(), e.traceStr().cc());
    }
    return -1;
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
        } catch(ZException err){
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
