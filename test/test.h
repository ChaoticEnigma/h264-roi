#ifndef TEST_H
#define TEST_H

#include "zlog.h"

using namespace std;
using namespace LibChaos;

int array_block();
int assoc_block();

int string_block();
int string_magic_block();
int path_block();
int path_windows_block();

int file_block();

int thread_block();

int autobuffer_block();

int socket_test();

int error_block();

typedef int (*test_func)(void);

#endif // TEST_H
