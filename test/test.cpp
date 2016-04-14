#include "test.h"
#include "zexception.h"

#include <stdio.h>

int runTests(ZAssoc<ZString, Test> tests);

void addTest(Test &test, ZMap<ZString, Test> &testmap, ZList<Test> &testlistout, ZMap<ZString, Test> &testmapout){
    // Check for dependencies
    for(auto j = test.deps.begin(); j.more(); ++j){
        if(testmap.contains(j.get()))
            addTest(testmap[j.get()], testmap, testlistout, testmapout);
    }
    if(!testmapout.contains(test.name)){
        testlistout.push(test);
        //testmapout[test.name] = test;
        testmapout.add(test.name, test);
    }
}

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

        // Test registration functions
        ZList<reg_func> regtests = {
            allocator_tests,
            pointer_tests,

            binary_tests,
            array_tests,
            list_tests,

            string_tests,
            path_tests,
            sandbox_tests
        };

        // List tests
        ZList<Test> alltests;
        for(auto i = regtests.begin(); i.more(); ++i)
            alltests.append(i.get()());

        // Build map
        ZMap<ZString, Test> testmap;
        for(auto i = alltests.begin(); i.more(); ++i)
            testmap.add(i.get().name, i.get());

        // Resolve dependencies
        ZMap<ZString, Test> testm;
        ZList<Test> tests;
        for(auto i = alltests.begin(); i.more(); ++i){
            addTest(i.get(), testmap, tests, testm);
        }

        // Run tests
        for(auto i = alltests.begin(); i.more(); ++i){
            Test test = i.get();
            ZString status = "PASS";

            ZLogWorker::setStdOutEnable(false);
            try {
                test.func();
            } catch(int e){
                status = ZString("FAIL: line ") + e;
            } catch(ZException e){
                status = ZString("FAIL: ") + e.what();
            }
            ZLogWorker::setStdOutEnable(true);

            ZString deps;
            for(auto j = test.deps.begin(); j.more(); ++j){
                deps += j.get();
                deps += ",";
            }
            deps.substr(deps.size()-1);

            LOG(test.name.pad(' ', 30) << status);
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
