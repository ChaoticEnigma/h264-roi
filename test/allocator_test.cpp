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

    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    stor->resize(10);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    stor->resize(11);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    stor->resize(60);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());

    delete stor;

    return 0;
}
