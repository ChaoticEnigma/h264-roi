#include "test.h"

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZPath lgf = ZString("logs") + ZLog::genLogFileName("testchaos_");
    ZLog::addLogFile(lgf, ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::addLogFile(lgf, ZLogSource::debug, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZLog::addLogFile(lgf, ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");


    ZAssoc<ZString, test_func> tests;
    tests["array"] = array_block;
    tests["assoc"] = assoc_block;
    tests["string"] = string_block;
    tests["string_magic"] = string_magic_block;
    tests["path"] = path_block;
    tests["path_windows"] = path_windows_block;
    tests["file"] = file_block;
    tests["thread"] = thread_block;
    tests["autobuffer"] = autobuffer_block;
#ifdef BUILD_ZSOCKET
    tests["socket"] = socket_test;
    tests["socketserver"] = socketserver_test;
    tests["socket2"] = socket2_test;
#endif
#if PLATFORM == LINUX
    tests["error"] = error_block;
#endif

    if(argc > 1){
        ArZ run;
        ZString runstr;
        for(int i = 1; i < argc; ++i){
            run.push(argv[i]);
            runstr << "'" << argv[i] << "',";
        }
        runstr.substr(0, runstr.size()-1);

        LOG("*** Starting Tests " << runstr);
        for(zu64 i = 0; i < run.size(); ++i){
            bool ok = false;
            for(zu64 j = 0; j < tests.size(); ++j){
                if(tests.key(j) == run[i].toLower()){
                    if(tests[j]() != 0){
                        LOG("!!! Test '" << tests.key(j) << "' Failed!");
                        return i;
                    } else {
                        LOG("=== Finished Test '" << tests.key(j) << "'");
                        ok = true;
                        break;
                    }
                }
            }
            if(!ok)
                LOG("No such test '" << run[i] << "'!");
        }
        LOG("*** Finished Tests " << runstr);
    } else {
        LOG("*** Starting All LibChaos Tests");
        for(zu64 i = 0; i < tests.size(); ++i){
            if(tests[i]() != 0){
                LOG("!!! Test '" << tests.key(i) << "' Failed!");
                return i;
            }
            LOG("=== Finished Test '" << tests.key(i) << "'");
        }
        LOG("*** All LibChaos Tests Done.");
    }
    return 0;
}
