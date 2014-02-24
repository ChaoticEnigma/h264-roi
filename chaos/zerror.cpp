#include "zerror.h"
#include "zlog.h"

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

void exitHandler(int sig){
    LOG("Exit Handler " << sig);
}
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
bool registerExitHandler(){
#if PLATFORM == LINUX
    struct sigaction action;
    action.sa_handler = exitHandler;
    sigemptyset(&action.sa_mask);
    //sigaddset(&action.sa_mask, SIGTERM);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, 0);
    sigaction(SIGTERM, &action, 0);
    sigaction(SIGQUIT, &action, 0);
#elif PLATFORM == WINDOWS
    if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE)){
        return false;
    }
#endif
    return true;
}

}
}
