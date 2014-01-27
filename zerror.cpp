#include "zerror.h"

#include "zlog.h"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

//#include "demangle.h"

namespace LibChaos {
namespace ZError {

void handler(int sig) {
    ELOG("Error: signal " << sig);
    void *buffer[100];
    int nptrs = backtrace(buffer, 100);
    char **strings = backtrace_symbols(buffer, nptrs);
    if(strings != NULL){
        for(int j = 0; j < nptrs; ++j){
            //ELOG(strings[j]);
            ZString tmp = strings[j];
            ZString exec = ZPath(ZString::substr(tmp, 0, ZString::findFirst(tmp, "("))).last().str();
            tmp.substr(ZString::findFirst(tmp, "("));
            ArZ pts = tmp.explode(' ');
            ArZ fpts = pts[0].strip('(').strip(')').explode('+');
            ZString addr = pts[1].strip('[').strip(']');
            ZString faddr = fpts[1];
            ZString fname = fpts[0];

            //fname = cplus_demangle(fname.cc(), 0);

            ELOG(exec << " - (" << fname << " " << faddr << ") : [" << addr << "]");
        }
        free(strings);
    }
    exit(sig);
}

void registerSigSegv(){
    signal(SIGSEGV, handler);
}

}
}
