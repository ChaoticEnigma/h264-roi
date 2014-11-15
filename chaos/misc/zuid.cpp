#include "zuid.h"

#include <time.h>

namespace LibChaos {

ZUID::ZUID(){
    clock_t clicks = clock();
    time_t rawtime = time(NULL);
    tm time = gmtime(rawtime);

}

ZString ZUID::str(){

}


}
