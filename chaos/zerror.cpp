#include "zerror.h"
#include "zlog.h"
#include "zmap.h"

#if PLATFORM == LINUX
    #include <execinfo.h>
    #include <signal.h>
#elif PLATFORM == WINDOWS
    #include <stdlib.h>
    #include <windows.h>
    #include <imagehlp.h>
#endif

namespace LibChaos {
namespace ZError {

#if PLATFORM == LINUX
void fatalSignalHandler(int sig){
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
#elif PLATFORM == WINDOWS
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
#endif

void registerSigSegv(){
#if PLATFORM == LINUX
    signal(SIGSEGV, fatalSignalHandler);
#endif
}

struct sigset {
    zerror_signal sigtype;
    signalHandler handler;
};
static ZMap<int, sigset> sigmap;

#if PLATFORM == LINUX

void sigHandle(int sig){
    if(sigmap.exists(sig) && sigmap[sig].handler != NULL)
        (sigmap[sig].handler)(sigmap[sig].sigtype);
}

#elif PLATFORM == WINDOWS

BOOL WINAPI ConsoleHandler(DWORD dwType){
    LOG("Console Exit Handler " << dwType);
//    switch(dwType){
//    case CTRL_C_EVENT:
//        printf("ctrl-c\n");
//        break;
//    case CTRL_BREAK_EVENT:
//        printf("break\n");
//        break;
//    default:
//        printf("Some other event\n");
//        break;
//    }
    return TRUE;
}

#endif

bool registerSignalHandler(zerror_signal sigtype, signalHandler handler){

#if PLATFORM == LINUX

    int sig = 0;
    switch(sigtype){
    case interrupt:
        sig = SIGINT;
        break;
    case abort:
        sig = SIGABRT;
        break;
    case quit:
        sig = SIGQUIT;
        break;
    case illegal:
        sig = SIGILL;
        break;
    case segv:
        sig = SIGSEGV;
        break;
    case terminate:
        sig = SIGTERM;
        break;
    case fpe:
        sig = SIGFPE;
        break;
    default:
        return false;
        break;
    }

    sigmap[sig] = { sigtype, handler };

    struct sigaction action;
    action.sa_handler = sigHandle;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, sig);
    action.sa_flags = 0;
    sigaction(sig, &action, 0);

#elif PLATFORM == WINDOWS

    if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE)){
        return false;
    }

#endif

    return true;
}
bool registerInterruptHandler(signalHandler handler){
    return registerSignalHandler(interrupt, handler);
}

}
}
