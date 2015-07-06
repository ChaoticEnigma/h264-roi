#include "test.h"
#include "zpdf.h"

int pdf_test(){

    ZPath path = "E:/Desktop/CCO07042015.pdf";
    ZPDF pdf1(path);
    pdf1.list();

    return 0;
}
