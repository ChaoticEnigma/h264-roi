#include "test.h"
#include "zexception.h"

#include <stdio.h>

int runTests(ZAssoc<ZString, Test> tests);

void addTest(Test &test, ZMap<ZString, Test*> &testmap, ZArray<Test> &testout, ZMap<ZString, Test> &testmapout){
    // Check for dependencies
    for(auto i = test.deps.begin(); i.more(); ++i){
        if(testmap.contains(*i)){
            addTest(*testmap[*i], testmap, testout, testmapout);
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
            misc_tests,
            number_tests,

            file_tests,
            image_tests,
            pdf_tests,

            socket_tests,

            thread_tests,
            error_tests,

            sandbox_tests
        };

        // List tests
        ZArray<Test> alltests;
        for(auto i = regtests.begin(); i.more(); ++i)
            alltests.append(i.get()());

        // Build map
        ZMap<ZString, Test*> testmap;
        for(auto i = alltests.begin(); i.more(); ++i)
            testmap.add(i.get().name, &i.get());

        // Command line arguments
        bool hideout = true;
        for(int i = 1; i < argc; ++i){
            ZString arg = argv[i];
            if(arg == "++"){
                for(auto j = alltests.begin(); j.more(); ++j)
                    j.get().run = true;
            } else if(arg == "--"){
                for(auto j = alltests.begin(); j.more(); ++j)
                    j.get().run = false;
            } else if(arg.beginsWith("+")){
                ZString name = ZString::substr(arg, 1);
                if(testmap.contains(name))
                    testmap[name]->run = true;
                else
                    LOG("No test: " << name);
            } else if(arg.beginsWith("-")){
                ZString name = ZString::substr(arg, 1);
                if(testmap.contains(name))
                    testmap[name]->run = false;
                else
                    LOG("No test: " << name);
            } else if(testmap.contains(arg)){
                if(testmap.contains(arg)){
                    for(auto j = alltests.begin(); j.more(); ++j)
                        j.get().run = false;
                    testmap[arg]->run = true;
                    hideout = false;
                } else {
                    LOG("No test: " << arg);
                }
            }
        }

        // Resolve dependencies
        ZArray<Test> tests;
        ZMap<ZString, Test> testm;
        for(auto i = alltests.begin(); i.more(); ++i){
            if(i.get().run)
                addTest(i.get(), testmap, tests, testm);
        }

        // Run tests
        ZMap<ZString, int> teststatus;
        for(zu64 i = 0; i < tests.size(); ++i){
            Test test = tests[i];
            ZString status = " PASS";

            bool skip = false;
            for(auto j = test.deps.begin(); j.more(); ++j){
                ZString name = *j;
                if(teststatus.contains(*j) && teststatus[*j] == 2){
                    skip = true;
                    status = ZString("-SKIP: ") + *j;
                }
            }

            ZClock clock;
            LOG("* " << ZString(i).lpad(' ', 2) << " " << ZString(test.name).pad(' ', 30) << (hideout ? ZLog::NOLN : ZLog::NEWLN));

            if(!skip && test.func){
                if(hideout)
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
                if(hideout)
                    ZLogWorker::setStdOutEnable(true);
            }

            ZString result = status;
            if(teststatus[test.name] == 1)
                result = result + " [" + clock.getSecs() + "]";

            if(hideout)
                RLOG(result << ZLog::NEWLN);
            else
                LOG("* " << ZString(i).lpad(' ', 2) << " " << ZString(test.name).pad(' ', 30) << status);

        }

        return 0;
    } catch(ZException e){
        printf("Catastrophic Failure: %s - %d\n%s\n", e.what().cc(), e.code(), e.traceStr().cc());
//    } catch(zexception e){
//        printf("CATACLYSMIC FAILURE: %s\n", e.what);
    } catch(zallocator_exception e){
        printf("Allocator Failure: %s\n", e.what);
    }
    return -1;
}
