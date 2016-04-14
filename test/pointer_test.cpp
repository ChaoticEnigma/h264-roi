#include "test.h"
#include "zpointer.h"
#include <stdio.h>

struct AnObject {
    AnObject(ZString s){
        str = s;
        LOG("Object created!");
    }
    ~AnObject(){
        LOG("Object destroyed!");
    }

    ZString str;
};

ZPointer<AnObject> aptr;

ZPointer<AnObject> gptr;

void a(ZPointer<AnObject> &sptr, ZPointer<AnObject> ptr){
    sptr = ptr;
    //sptr.swap(ptr);
}

void pointer(){

    ZPointer<AnObject> sptr;

    {
        ZPointer<AnObject> ptr(new AnObject("this is a string"));
        LOG(ptr.ptr()->str << " " << ptr.count());
        a(sptr, ptr);
        LOG(ptr.ptr()->str << " " << ptr.count());
        a(aptr, ptr);
        LOG(ptr.ptr()->str << " " << ptr.count());
    }

    LOG(sptr.ptr()->str << " " << sptr.count());

    gptr = sptr;

    LOG(sptr.ptr()->str << " " << sptr.count());

    gptr.release();

    LOG(sptr.ptr()->str << " " << sptr.count());

    aptr.release();
}

ZList<Test> pointer_tests(){
    return {
        { "pointer", pointer, true, {} },
    };
}
