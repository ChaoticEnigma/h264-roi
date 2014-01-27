#include "test.h"

#include "zerror.h"

int error_block(){
    LOG("=== Error Test...");
    ZError::registerSigSegv();

    char *s = (char *)"Hello World!";
    *s = 'H';
    //bye bye
    return 0;
}
