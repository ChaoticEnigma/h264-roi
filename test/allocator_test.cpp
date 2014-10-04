#include "test.h"
#include "zallocator.h"
#include "zstorage.h"

int allocator_block(){

//    LOG("alloc char");
//    ZAllocator<char> alloc;
//    char *test = alloc.alloc(20);
//    LOG("free char");
//    alloc.free();

//    LOG("Alloc void");
//    ZAllocator<void> alloc2;
//    void *test2 = alloc2.alloc(20);
//    LOG("free void");
//    alloc2.free();

    ZStorage *stor = new ZDefaultStorage;

    auto print = [&](){
        ZString str = "[";
        for(zu64 i = 0; i < stor->size(); ++i)
            str += ZString((zu64)stor->get(i)) + ",";
        str.substr(0, str.size()-1);
        str += "]";
        LOG(str);
    };

    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());

    stor->resize(10);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 0; i < stor->size(); ++i)
        stor->get(i) = (zbyte)i;
    print();

    stor->resize(11);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    print();

    stor->resize(12);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    print();

    stor->resize(21);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    print();
    for(zu64 i = 0; i < stor->size(); ++i)
        stor->get(i) = (zbyte)i;
    print();

    stor->resize(90);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 0; i < stor->size(); ++i)
        stor->get(i) = (zbyte)i;
    print();

    stor->resize(40);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    print();

    stor->resize(350);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 0; i < stor->size(); ++i)
        stor->get(i) = (zbyte)i;
    print();

    delete stor;

    return 0;
}
