#include "test.h"
#include "zsocket.h"

int tcp_test(){
    LOG("=== TCP Socket Test...");

    ZSocket sock;
    //sock.openStream(ZAddress(0, 8998));
    sock.openStream(ZAddress("www.google.com"));
    sock.openStream(ZAddress("www.znnxs.com"));

    return 0;
}
int tcpserver_test(){
    LOG("=== TCP Socket Server Test...");

    return 0;
}
