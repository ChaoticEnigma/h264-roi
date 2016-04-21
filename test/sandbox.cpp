#include "test.h"

namespace LibChaosTest {

void sandbox(){

}

ZArray<Test> sandbox_tests(){
    return {
        { "sandbox", sandbox, false, {} }
    };
}

}
