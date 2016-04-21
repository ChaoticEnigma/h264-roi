#include "test.h"
#include "zpointer.h"

namespace LibChaosTest {

bool destroyed = false;

struct AnObject {
    AnObject(ZString s){
        str = s;
        LOG("Object created!");
    }
    ~AnObject(){
        destroyed = true;
        LOG("Object destroyed!");
    }

    ZString str;
};

ZPointer<AnObject> aptr;

ZPointer<AnObject> gptr;

void a(ZPointer<AnObject> &sptr, ZPointer<AnObject> ptr, zu64 c){
    LOG(ptr.ptr()->str << " " << ptr.count());
    TASSERT(ptr.ptr()->str == "test string");
    TASSERT(ptr.count() == c+1);

    sptr = ptr;
    TASSERT(sptr.ptr()->str == "test string");
    TASSERT(sptr.count() == c+2);

    LOG(ptr.ptr()->str << " " << ptr.count());
    TASSERT(ptr.ptr()->str == "test string");
    TASSERT(ptr.count() == c+2);
}

void pointer(){
    {
        ZPointer<AnObject> sptr;
        TASSERT(sptr.ptr() == nullptr);
        TASSERT(sptr.count() == 0);

        {
            ZPointer<AnObject> ptr(new AnObject("test string"));

            LOG(ptr.ptr()->str << " " << ptr.count());
            TASSERT(ptr.ptr()->str == "test string");
            TASSERT(ptr.count() == 1);

            a(sptr, ptr, 1);
            LOG(ptr.ptr()->str << " " << ptr.count());
            TASSERT(ptr.ptr()->str == "test string");
            TASSERT(ptr.count() == 2);

            a(aptr, ptr, 2);
            LOG(ptr.ptr()->str << " " << ptr.count());
            TASSERT(ptr.ptr()->str == "test string");
            TASSERT(ptr.count() == 3);
        }

        LOG(sptr.ptr()->str << " " << sptr.count());
        TASSERT(sptr.ptr()->str == "test string");
        TASSERT(sptr.count() == 2);

        gptr = sptr;
        LOG(sptr.ptr()->str << " " << sptr.count());
        TASSERT(sptr.ptr()->str == "test string");
        TASSERT(sptr.count() == 3);

        gptr.release();
        LOG(sptr.ptr()->str << " " << sptr.count());
        TASSERT(sptr.ptr()->str == "test string");
        TASSERT(sptr.count() == 2);

        aptr.release();
        TASSERT(sptr.ptr()->str == "test string");
        TASSERT(sptr.count() == 1);

        TASSERT(!destroyed);
    }

    TASSERT(destroyed);
}

ZArray<Test> pointer_tests(){
    return {
        { "pointer", pointer, true, {} },
    };
}

}
