#include "test.h"
#include "zexception.h"

#include <stdio.h>

int runTests(ZAssoc<ZString, Test> tests);

int main(int argc, char **argv){
    try {
        //ZLog::init(); // BUG: threaded zlog sometimes crashes
        ZLog::logLevelStdOut(ZLog::INFO, "%clock% %thread% N %log%");
        ZLog::logLevelStdOut(ZLog::DEBUG, "\x1b[35m%clock% %thread% D %log%\x1b[m");
        ZLog::logLevelStdErr(ZLog::ERROR, "\x1b[31m%clock% %thread% E [%function%|%file%:%line%] %log%\x1b[m");
        ZPath lgf = ZPath("logs") + ZLog::genLogFileName("testchaos_");
        ZLog::logLevelFile(ZLog::INFO, lgf, "%time% %thread% N %log%");
        ZLog::logLevelFile(ZLog::DEBUG, lgf, "%time% %thread% D [%function%|%file%:%line%] %log%");
        ZLog::logLevelFile(ZLog::ERROR, lgf, "%time% %thread% E [%function%|%file%:%line%] %log%");

        LOG("Testing LibChaos: " << LibChaosDescribe());

        ZList<reg_func> regtests = {
            string_tests,
            sandbox_tests
        };

        ZList<Test> alltests;
        for(auto it = regtests.begin(); it.more(); ++it)
            alltests.append(it.get()());

        for(auto it = alltests.begin(); it.more(); ++it){
            ZString status = "PASS";

            ZLogWorker::setStdOutEnable(false);
            try {
                it.get().func();
            } catch(int e){
                status = ZString("FAIL: ") + e;
            } catch(ZException e){
                status = ZString("FAIL: ") + e.what();
            }
            ZLogWorker::setStdOutEnable(true);

            LOG(it.get().name.pad(' ', 30) << status);
        }

        return 0;

//        ZMap<ZString, Test> alltests = {
//            { "allocator",  { allocator_test,   true } },
//            { "pointer",    { pointer_test,     true } },
//            { "storage",    { storage_test,     true } },

//            { "binary",     { binary_test,      true } },

//            { "array",      { array_test,       true } },
//            { "assoc",      { assoc_test,       true } },
//            { "stack",      { stack_test,       true } },
//            { "list",       { list_test,        true } },
//            { "queue",      { queue_test,       true } },

//            { "graph",      { graph_test,       false } },

//            { "hash",       { hash_test,        true } },
//            { "set",        { set_test,         true } },
//            { "map",        { map_test,         true } },

//            { "string",     { string_test,      true } },
//            { "path",       { path_test,        true } },
//            { "json",       { json_test,        true } },

//            { "thread",     { thread_test,      true } },
//            { "mutex",      { mutex_test,       true } },

//            { "number",     { number_test,      true } },
//            { "uid",        { uid_test,         true } },

//            { "file",       { file_test,        true } },
//            { "image",      { image_test,       true } },
//            { "pdf",        { pdf_test,         true } },

//            { "udp",        { udp_test,         false } },
//            { "udpserver",  { udpserver_test,   false } },
//            { "tcp",        { tcp_test,         false } },
//            { "tcpserver",  { tcpserver_test,   false } },
//            { "tcpserver2", { tcpserver_test2,  false } },
//            { "tcpserver3", { tcpserver_test3,  false } },

//            { "sandbox",    { sandbox,          false } },

//            { "error",      { error_test,       false } },
//        };

//        bool all = false;
//        if(argc > 1){
//            if(ZString(argv[1]) == "all"){
//                all = true;
//            } else {
//                for(int i = 1; i < argc; ++i){
//                    ZString key = argv[i];
//                    if()
//                     if(alltests.exists(key)){
//                        runtests.push(key, alltests[key]);
//                        runtests[key].run = true;
//                        runstr << key << ",";
//                    } else {
//                        LOG("No Test " << key << ", Ignoring");
//                    }
//                }
//            }
//            runstr.strip(',');
//        } else {
//            all = true;
//        }

//        if(all){
//            runtests = alltests;
//            runstr = "All";
//        }

//        LOG("*** Starting Tests \"" << runstr << "\"");
//        int ret = runTests(runtests);
//        LOG("*** Finished Tests \"" << runstr << "\"");
//        return ret;

    } catch(ZException e){
        printf("Catastrophic Failure: %s - %d\n%s\n", e.what().cc(), e.code(), e.traceStr().cc());
    } catch(zexception e){
        printf("CATACLYSMIC FAILURE: %s\n", e.what);
    } catch(zallocator_exception e){
        printf("Allocator Failure: %s\n", e.what);
    }
    return -1;
}
