#include "test.h"
#include "zallocator.h"
#include "zstorage.h"

int allocator_test(){
    LOG("alloc char");
    ZAllocator<char> alloc;
    char *test = alloc.alloc(20);
    LOG("free char");
    alloc.dealloc(test);

    LOG("Alloc void");
    ZAllocator<void> alloc2;
    void *test2 = alloc2.alloc(20);
    LOG("free void");
    alloc2.dealloc(test2);

    return 0;
}
