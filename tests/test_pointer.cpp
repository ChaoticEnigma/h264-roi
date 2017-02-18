#include "tests.h"
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
    LOG(ptr.get()->str << " " << ptr.count());
    TASSERT(ptr.get()->str == "test string");
    TASSERT(ptr.count() == c+1);

    sptr = ptr;
    TASSERT(sptr.get()->str == "test string");
    TASSERT(sptr.count() == c+2);

    LOG(ptr.get()->str << " " << ptr.count());
    TASSERT(ptr.get()->str == "test string");
    TASSERT(ptr.count() == c+2);
}

void pointer(){
    {
        ZPointer<AnObject> sptr;
        TASSERT(sptr.get() == nullptr);
        TASSERT(sptr.count() == 0);

        {
            ZPointer<AnObject> ptr(new AnObject("test string"));

            LOG(ptr.get()->str << " " << ptr.count());
            TASSERT(ptr.get()->str == "test string");
            TASSERT(ptr.count() == 1);

            a(sptr, ptr, 1);
            LOG(ptr.get()->str << " " << ptr.count());
            TASSERT(ptr.get()->str == "test string");
            TASSERT(ptr.count() == 2);

            a(aptr, ptr, 2);
            LOG(ptr.get()->str << " " << ptr.count());
            TASSERT(ptr.get()->str == "test string");
            TASSERT(ptr.count() == 3);
        }

        LOG(sptr.get()->str << " " << sptr.count());
        TASSERT(sptr.get()->str == "test string");
        TASSERT(sptr.count() == 2);

        gptr = sptr;
        LOG(sptr.get()->str << " " << sptr.count());
        TASSERT(sptr.get()->str == "test string");
        TASSERT(sptr.count() == 3);

        gptr.release();
        LOG(sptr.get()->str << " " << sptr.count());
        TASSERT(sptr.get()->str == "test string");
        TASSERT(sptr.count() == 2);

        aptr.release();
        TASSERT(sptr.get()->str == "test string");
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
