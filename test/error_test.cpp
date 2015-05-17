#include "test.h"
#include "zerror.h"

static int userwait = 0;

void intHandle(ZError::zerror_signal sig){
    if(sig == ZError::interrupt)
        userwait = 1;
    else if(sig == ZError::terminate)
        userwait = 2;
}

int trace_block();

int error_test(){
    trace_block();
    return 0;

    ZError::registerSigSegv();
    ZError::registerInterruptHandler(intHandle);
    ZError::registerSignalHandler(ZError::terminate, intHandle);

    LOG("Waiting for Interrupt or Terminate...");
    while(!userwait){
        // wait for ctrl+c
    }
    RLOG(ZLog::newln);
    if(userwait == 1)
        LOG("Caught Interrupt");
    else if(userwait == 2)
        LOG("Caught Terminate");

    return 0;
}

int trace_block(){

    ArZ trace = ZError::getStackTrace();

    for(zu64 i = 0; i < trace.size(); ++i){
        LOG(trace[i]);
    }

    return 0;
}

int exception_block(){

    return 0;
}
