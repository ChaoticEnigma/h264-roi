#include "test.h"
#include "zpointer.h"

ZPointer<ZString> sptr;

void a(ZPointer<ZString> ptr){
    sptr.swap(ptr);
}

int pointer_block(){

    {
        ZPointer<ZString> ptr(new ZString("this is a string"));
        LOG(*ptr.ptr());
        a(ptr);
    }

    LOG(*sptr.ptr());

    return 0;
}
