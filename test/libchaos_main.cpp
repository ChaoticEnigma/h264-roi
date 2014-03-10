#include "test.h"

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZPath lgf = ZPath("logs") + ZLog::genLogFileName("testchaos_");
    ZLog::addLogFile(lgf, ZLogSource::normal, "%time% %thread% - %log%");
    ZLog::addLogFile(lgf, ZLogSource::debug, "%time% %thread% %function% (%file%:%line%) - %log%");
    ZLog::addLogFile(lgf, ZLogSource::error, "%time% %thread% %function% (%file%:%line%) - %log%");

    ZAssoc<ZString, test_func> tests = {
        { "array", array_block },
        { "assoc", assoc_block },
        { "string", string_block },
        { "string_magic", string_magic_block },
        { "path", path_block },
        { "path_windows", path_windows_block },
        { "file", file_block },
        { "thread", thread_block },
        { "autobuffer", autobuffer_block },
        { "json", json_block },
        { "udp", udp_test },
        { "udpserver", udpserver_test },
        { "tcp", tcp_test },
        { "tcpserver", tcpserver_test },
        { "error", error_block }
    };

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
