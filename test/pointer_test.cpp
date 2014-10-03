#include "test.h"
#include "zpointer.h"

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

void a(ZPointer<AnObject> &sptr, ZPointer<AnObject> ptr){
    sptr = ptr;
    //sptr.swap(ptr);
}

int pointer_block(){

    ZPointer<AnObject> sptr;

    {
        ZPointer<AnObject> ptr(new AnObject("this is a string"));
        LOG(ptr.ptr()->str);
        a(sptr, ptr);
    }

    LOG(sptr.ptr()->str);

    return 0;
}
