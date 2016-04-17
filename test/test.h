#ifndef TEST_H
#define TEST_H

#include "zlog.h"
#include "zmap.h"

// Assert for Tests
#define TASSERT(X) if(!(X)) throw __LINE__;

using namespace LibChaos;

typedef void (*test_func)(void);

struct Test {
    ZString name;
    test_func func;
    bool run;
    ZArray<ZString> deps;
};

typedef ZArray<Test> (*reg_func)(void);

ZArray<Test> allocator_tests();
ZArray<Test> storage_tests();
ZArray<Test> pointer_tests();

ZArray<Test> binary_tests();
ZArray<Test> array_tests();
ZArray<Test> list_tests();

ZArray<Test> string_tests();
ZArray<Test> path_tests();
ZArray<Test> json_tests();

ZArray<Test> hash_tests();
ZArray<Test> graph_tests();
ZArray<Test> misc_tests();
ZArray<Test> number_tests();

ZArray<Test> file_tests();
ZArray<Test> image_tests();
ZArray<Test> pdf_tests();

ZArray<Test> socket_tests();

ZArray<Test> thread_tests();
ZArray<Test> error_tests();

ZArray<Test> sandbox_tests();

int test_forward_iterator(ZSimplexConstIterator<ZString> *it, zu64 size);
int test_reverse_iterator(ZDuplexIterator<ZString> *it, zu64 size);
int test_duplex_iterator(ZDuplexIterator<ZString> *it, zu64 size);
int test_random_iterator(ZRandomIterator<ZString> *it, zu64 size);

#endif // TEST_H
