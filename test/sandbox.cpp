#include "test.h"

void sandbox();

ZList<Test> sandbox_tests(){
    return {
        { "sandbox", sandbox, false }
    };
}

void sandbox(){

}
