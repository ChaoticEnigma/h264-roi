#include "test.h"
#include "zallocator.h"
#include "zstorage.h"

void allocator_char(){
    ZAllocator<char> alloc;
    char *test = alloc.alloc(20);
    alloc.dealloc(test);
}

void allocator_void(){
    ZAllocator<void> alloc2;
    void *test2 = alloc2.alloc(20);
    alloc2.dealloc(test2);
}

ZList<Test> allocator_tests(){
    return {
        { "allocator-char", allocator_char, true, {} },
        { "allocator-void", allocator_void, true, {} },
    };
}
