#include "test.h"
#include "zsocket.h"
#include <unistd.h>

void sendGrams(ZSocket *sock){
    zu64 count = 0;
    //ZAddress addr(127,0,0,1, 8998);
    //ZAddress addr(192,168,1,38, 8998);
    ZAddress addr(192,168,1,89, 8998);

    while(true){
        ZString str = "hello world out there! ";
        str << ZString::ItoS(count);
        ZBinary data((unsigned char *)str.cc(), str.size());
        sock->send(addr, data);
        LOG("to " << addr.str() << " (" << data.size() << "): \"" << data.raw() << "\"");
        ++count;
        usleep(500000);
    }
}

int socket_test(){
    LOG("=== Socket Test...");
    ZSocket sock;
    if(!sock.open(8998)){
        ELOG("Socket Open Fail");
        return 2;
    }

    LOG("Sending...");
    sendGrams(&sock);

    sock.close();
    return 0;
}

void receivedGram(ZAddress addr, ZBinary data){
    LOG("from " << addr.str() << " (" << data.size() << "): \"" << data << "\"");
}

int socketserver_test(){
    LOG("=== Socket Server Test...");
    ZSocket sock;
    if(!sock.open(8998)){
        ELOG("Socket Open Fail");
        return 2;
    }

    LOG("Listening...");
    sock.listen(receivedGram);

    sock.close();
    return 0;
}
