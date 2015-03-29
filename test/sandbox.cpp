#include "test.h"

#include <iostream>

zu64 problemA(zu64 days, zu64 shirts, zu64 max_days){ // 7 4 3 = 2
    zu64 clean = 0;
    zu64 left = shirts;
    while((days-clean) / max_days <= left){
        ++clean;
        --left;
    }
    return clean;
}

int sandbox(){
    LOG(problemA(7, 4, 2));
    LOG(problemA(7, 4, 3));
    LOG(problemA(7, 2, 4));

    return 0;
}
