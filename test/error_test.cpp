#include "test.h"
#include "zexception.h"

static int userwait = 0;

void intHandle(ZException::zerror_signal sig){
    if(sig == ZException::interrupt)
        userwait = 1;
    else if(sig == ZException::terminate)
        userwait = 2;
}

int error_block(){
    ZException::registerSigSegv();
    ZException::registerInterruptHandler(intHandle);
    ZException::registerSignalHandler(ZException::terminate, intHandle);

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
