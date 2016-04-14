#ifndef TEST_H
#define TEST_H

#include "zlog.h"
#include "zmap.h"

// Fail Current Test
#define FAILTEST throw __LINE__;
// Assert for Tests
#define TASSERT(X) if(!(X)) throw __LINE__;

using namespace LibChaos;

typedef void (*test_func)(void);

struct Test {
    //Test(const Test &t) : name(t.name), func(t.func), run(t.run), deps(t.deps){}
    //Test(const Test &t) : name(t.name), func(t.func), run(t.run), deps(t.deps){}
    ZString name;
    test_func func;
    bool run;
    ZList<ZString> deps;
};

typedef ZList<Test> (*reg_func)(void);

ZList<Test> allocator_tests();
ZList<Test> pointer_tests();

ZList<Test> binary_tests();
ZList<Test> array_tests();
ZList<Test> list_tests();

ZList<Test> string_tests();
ZList<Test> path_tests();

ZList<Test> sandbox_tests();

int test_forward_iterator(ZSimplexConstIterator<ZString> *it, zu64 size);
int test_reverse_iterator(ZDuplexIterator<ZString> *it, zu64 size);
int test_duplex_iterator(ZDuplexIterator<ZString> *it, zu64 size);
int test_random_iterator(ZRandomIterator<ZString> *it, zu64 size);

int list_test();
int queue_test();

int hash_test();
int set_test();
int map_test();

int graph_test();

int binary_test();

int json_test();

int storage_test();

int thread_test();
int mutex_test();

int number_test();

int random_test();
int uid_test();

int file_test();
int image_test();
int pdf_test();

int udp_test();
int udpserver_test();

int tcp_test();
int tcpserver_test();
int tcpserver_test2();
int tcpserver_test3();

int error_test();

#endif // TEST_H
