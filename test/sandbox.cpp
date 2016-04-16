#include "test.h"

void sandbox(){

}

ZArray<Test> sandbox_tests(){
    return {
        { "sandbox", sandbox, false, {} }
    };
}
