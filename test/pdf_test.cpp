#include "test.h"
#include "zpdf.h"

void pdf_read(){
    ZPath path = "E:/Desktop/CCO07042015.pdf";
    ZPDF pdf1(path);
    pdf1.list();
}
