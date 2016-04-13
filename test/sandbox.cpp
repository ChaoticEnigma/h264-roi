#include "test.h"

int sandbox();

ZList<Test> sandbox_tests(){
    return {
        { "sandbox", sandbox, false }
    };
}

int sandbox(){

    return 0;
}
