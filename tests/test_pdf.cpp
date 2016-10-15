#include "tests.h"
#include "zpdf.h"

namespace LibChaosTest {

void pdf_read(){
    ZPath path = "E:/Desktop/CCO07042015.pdf";
    ZPDF pdf1(path);
    pdf1.list();
}

ZArray<Test> pdf_tests(){
    return {
        { "pdf-read", pdf_read, false, {} }
    };
}

}
