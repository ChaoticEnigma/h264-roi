#include "test.h"
#include "zerror.h"

static int userwait = 0;

void intHandle(ZError::zerror_signal sig){
    if(sig == ZError::interrupt)
        userwait = 1;
    else if(sig == ZError::terminate)
        userwait = 2;
}

void error_signal(){
    ZError::registerSigSegv();
    ZError::registerInterruptHandler(intHandle);
    ZError::registerSignalHandler(ZError::terminate, intHandle);

    LOG("Waiting for Interrupt or Terminate...");
    while(!userwait){
        // wait for ctrl+c
    }
    RLOG(ZLog::NEWLN);
    if(userwait == 1)
        LOG("Caught Interrupt");
    else if(userwait == 2)
        LOG("Caught Terminate");
}

void error_trace(){
    ArZ trace = ZError::getStackTrace();

    for(zu64 i = 0; i < trace.size(); ++i){
        LOG(trace[i]);
    }
}

void error_exception(){

}

ZArray<Test> error_tests(){
    return {
        { "error-signal",       error_signal,       false, {} },
        { "error-trace",        error_trace,        false, {} },
        { "error-execption",    error_exception,    false, {} },
    };
}
