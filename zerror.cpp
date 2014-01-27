#include "zerror.h"

#include "zlog.h"
//#include <stdio.h>
//#include <execinfo.h>
#include <signal.h>
//#include <stdlib.h>
//#include <unistd.h>

//#include "demangle.h"

#if PLATFORM == WINDOWS
//    #include <stdlib.h>
//    #include <windows.h>
//    #include <imagehlp.h>
#endif

namespace LibChaos {
namespace ZError {

#if PLATFORM == WINDOWS
//void windows_print_stacktrace(CONTEXT *context){
//    SymInitialize(GetCurrentProcess(), 0, true);
//    STACKFRAME frame = { 0 };
//    /* setup initial stack frame */
//    frame.AddrPC.Offset         = context->Eip;
//    frame.AddrPC.Mode           = AddrModeFlat;
//    frame.AddrStack.Offset      = context->Esp;
//    frame.AddrStack.Mode        = AddrModeFlat;
//    frame.AddrFrame.Offset      = context->Ebp;
//    frame.AddrFrame.Mode        = AddrModeFlat;

//    TCHAR szExeFileName[MAX_PATH];
//    GetModuleFileName(NULL, szExeFileName, MAX_PATH);

//    while(StackWalk(IMAGE_FILE_MACHINE_AMD64,
//            GetCurrentProcess(),
//            GetCurrentThread(),
//            &frame,
//            context,
//            0,
//            SymFunctionTableAccess,
//            SymGetModuleBase,
//            0)){
//        addr2line(szExeFileName, (void*)frame.AddrPC.Offset);
//    }
//    SymCleanup( GetCurrentProcess() );
//}
#else
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
#endif

void registerSigSegv(){
#if PLATFORM == LINUX
    signal(SIGSEGV, handler);
#endif
}

}
}
