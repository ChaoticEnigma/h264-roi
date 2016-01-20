#include "test.h"
#include "zexception.h"

#include <stdio.h>

struct Test {
    test_func func;
    bool run;
};

int runTests(ZAssoc<ZString, Test> tests);

int main(int argc, char **argv){
    try {
        //ZLog::init(); // BUG: threaded zlog sometimes crashes
        ZLog::formatStdout(ZLogSource::NORMAL, "%clock% %thread% - %log%");
        ZLog::formatStdout(ZLogSource::DEBUG, "%clock% %thread% - %log%");
        ZLog::formatStderr(ZLogSource::ERRORS, "%clock% %thread% %function% (%file%:%line%) - %log%");
        ZPath lgf = ZPath("logs") + ZLog::genLogFileName("testchaos_");
        ZLog::addLogFile(lgf, ZLogSource::NORMAL, "%time% %thread% - %log%");
        ZLog::addLogFile(lgf, ZLogSource::DEBUG, "%time% %thread% %function% (%file%:%line%) - %log%");
        ZLog::addLogFile(lgf, ZLogSource::ERRORS, "%time% %thread% %function% (%file%:%line%) - %log%");

        LOG("Testing LibChaos: " << LibChaosDescribe());

        ZAssoc<ZString, Test> alltests = {
            { "allocator",  { allocator_test,   true } },
            { "pointer",    { pointer_test,     true } },
            { "storage",    { storage_test,     true } },

            { "binary",     { binary_test,      true } },

            { "array",      { array_test,       true } },
            { "assoc",      { assoc_test,       true } },
            { "stack",      { stack_test,       true } },
            { "list",       { list_test,        true } },
            { "queue",      { queue_test,       true } },

            { "graph",      { graph_test,       false } },

            { "hash",       { hash_test,        true } },
            { "set",        { set_test,         true } },
            { "map",        { map_test,         true } },

            { "string",     { string_test,      true } },
            { "path",       { path_test,        true } },
            { "json",       { json_test,        true } },

            { "thread",     { thread_test,      true } },
            { "mutex",      { mutex_test,       true } },

            { "number",     { number_test,      true } },
            { "uid",        { uid_test,         true } },

            { "file",       { file_test,        true } },
            { "image",      { image_test,       true } },
            { "pdf",        { pdf_test,         true } },

            { "udp",        { udp_test,         false } },
            { "udpserver",  { udpserver_test,   false } },
            { "tcp",        { tcp_test,         false } },
            { "tcpserver",  { tcpserver_test,   false } },
            { "tcpserver2", { tcpserver_test2,  false } },
            { "tcpserver3", { tcpserver_test3,  false } },

            { "sandbox",    { sandbox,          false } },

            { "error",      { error_test,       false } },
        };

        ZAssoc<ZString, Test> runtests;
        ZString runstr;
        bool all = false;
        if(argc > 1){
            if(ZString(argv[1]) == "all"){
                all = true;
            } else {
                for(int i = 1; i < argc; ++i){
                    ZString key = argv[i];
                     if(alltests.exists(key)){
                        runtests.push(key, alltests[key]);
                        runtests[key].run = true;
                        runstr << key << ",";
                    } else {
                        LOG("No Test " << key << ", Ignoring");
                    }
                }
            }
            runstr.strip(',');
        } else {
            all = true;
        }

        if(all){
            runtests = alltests;
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

int runTests(ZAssoc<ZString, Test> tests){
    ArZ errorstrings;
    for(zu64 i = 0; i < tests.size(); ++i){
        if(tests[i].run){
            LOG("=== Starting Test '" << tests.key(i) << "'...");
            int result = -1;
            try {
                result = tests[i].func();
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
    }
    if(!errorstrings.isEmpty()){
        for(zu64 i = 0; i < errorstrings.size(); ++i){
            LOG("ERR: " << errorstrings[i]);
        }
    }
    return 0;
}
