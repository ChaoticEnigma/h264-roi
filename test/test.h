#ifndef TEST_H
#define TEST_H

#include "zlog.h"

using namespace std;
using namespace LibChaos;

int sandbox();

int array_block();
int assoc_block();
int stack_block();
int list_block();
int queue_block();

int binary_block();

int string_block();
int path_block();

int pointer_block();
int allocator_block();
int storage_block();

int thread_block();

int number_block();

int file_block();
int png_block();

int autobuffer_block();

int udp_test();
int udpserver_test();

int tcp_test();
int tcpserver_test();
int tcpserver_test2();
int tcpserver_test3();

int json_block();

int error_block();

typedef int (*test_func)(void);

#endif // TEST_H
