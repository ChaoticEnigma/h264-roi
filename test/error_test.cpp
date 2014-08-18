#include "test.h"
#include "zerror.h"

static int userwait = 0;

void intHandle(ZError::zerror_signal sig){
    if(sig == ZError::interrupt)
        userwait = 1;
    else if(sig == ZError::terminate)
        userwait = 2;
}

int error_block(){
    LOG("=== Error Test...");
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
