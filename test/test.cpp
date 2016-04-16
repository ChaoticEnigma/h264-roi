#include "test.h"
#include "zexception.h"

#include <stdio.h>

int runTests(ZAssoc<ZString, Test> tests);

void addTest(Test &test, ZMap<ZString, Test> &testmap, ZArray<Test> &testout, ZMap<ZString, Test> &testmapout){
    // Check for dependencies
    for(auto i = test.deps.begin(); i.more(); ++i){
        if(testmap.contains(*i)){
            addTest(testmap[*i], testmap, testout, testmapout);
        } else {
            LOG("Skipping unknown dependency " << *i);
        }
    }
    // Add test
    if(!testmapout.contains(test.name)){
        testout.push(test);
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
        ZArray<reg_func> regtests = {
            allocator_tests,
            storage_tests,
            pointer_tests,

            binary_tests,
            array_tests,
            list_tests,

            string_tests,
            path_tests,
            json_tests,

            hash_tests,
            graph_tests,

            file_tests,
            image_tests,

            sandbox_tests
        };

        // List tests
        ZArray<Test> alltests;
        for(auto i = regtests.begin(); i.more(); ++i)
            alltests.append(i.get()());

        // Build map
        ZMap<ZString, Test> testmap;
        for(auto i = alltests.begin(); i.more(); ++i)
            testmap.add(i.get().name, i.get());

        // Resolve dependencies
        ZMap<ZString, Test> testm;
        ZArray<Test> tests;
        for(auto i = alltests.begin(); i.more(); ++i){
            addTest(i.get(), testmap, tests, testm);
        }

        // Run tests
        ZMap<ZString, int> teststatus;
        for(auto i = tests.begin(); i.more(); ++i){
            Test test = i.get();
            ZString status = " PASS";

            bool skip = false;
            for(auto j = test.deps.begin(); j.more(); ++j){
                if(teststatus.contains(*j) && teststatus[*j] == 2){
                    skip = true;
                    status = ZString("-SKIP: ") + *j;
                }
            }

            if(!skip){
                ZLogWorker::setStdOutEnable(false);
                try {
                    test.func();
                    teststatus[test.name] = 1;
                } catch(int e){
                    status = ZString("!FAIL: line ") + e;
                    teststatus[test.name] = 2;
                } catch(ZException e){
                    status = ZString("!FAIL: ") + e.what();
                    teststatus[test.name] = 2;
                }
                ZLogWorker::setStdOutEnable(true);
            }

            LOG(test.name.pad(' ', 30) << status);
        }

        return 0;
    } catch(ZException e){
        printf("Catastrophic Failure: %s - %d\n%s\n", e.what().cc(), e.code(), e.traceStr().cc());
    } catch(zexception e){
        printf("CATACLYSMIC FAILURE: %s\n", e.what);
    } catch(zallocator_exception e){
        printf("Allocator Failure: %s\n", e.what);
    }
    return -1;
}
