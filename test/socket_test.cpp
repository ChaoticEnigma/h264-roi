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
        data.data().push(0);
        LOG("to " << addr.str() << " (" << data.size() << "): \"" << (char *)data.raw() << "\"");
        sock->send(addr, data);
        ++count;
        usleep(500000);
    }
}

int socket_test(){
    LOG("=== Socket Test...");
    if(!ZSocket::InitializeSockets()){
        LOG("Init Error");
        return 1;
    }
    ZSocket sock;
    if(!sock.open(8998)){
        ELOG("Socket Open Fail");
        return 2;
    }

    LOG("Sending...");
    sendGrams(&sock);

    sock.close();
    ZSocket::ShutdownSockets();
    return 0;
}

void receivedGram(ZAddress addr, ZBinary data){
    LOG("from " << addr.str() << " (" << data.size() << "): \"" << (char *)data.raw() << "\"");
}

int socketserver_test(){
    LOG("=== Socket Server Test...");
    if(!ZSocket::InitializeSockets()){
        LOG("Init Error");
        return 1;
    }
    ZSocket sock;
    if(!sock.open(8998)){
        ELOG("Socket Open Fail");
        return 2;
    }

    LOG("Listening...");
    sock.listen(receivedGram);

    sock.close();
    ZSocket::ShutdownSockets();
    return 0;
}
