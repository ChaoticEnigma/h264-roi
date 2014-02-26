#include "test.h"
#include "zsocket.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "zerror.h"

static bool run = true;

void stopHandler(ZError::zerror_signal sig){
    run = false;
}

int udp_test(){
    LOG("=== UDP Socket Test...");
    ZError::registerInterruptHandler(stopHandler);
    ZError::registerSignalHandler(ZError::terminate, stopHandler);

    ZSocket sock(ZSocket::udp);
    sock.allowRebind(true);
    if(!sock.open(ZAddress(8998))){
        ELOG("Socket Open Fail");
        return 2;
    }

    LOG("Sending...");

    ZAddress addr("::0:1", 8998);
    //ZAddress addr("192.168.1.38", 8998);
    //ZAddress addr("192.168.1.89", 8998);

    for(zu64 i = 0; run && i < 5; ++i){
        ZString str = "hello world out there! ";
        str << ZString::ItoS(i);
        ZBinary data((unsigned char *)str.cc(), str.size());
        sock.send(addr, data);
        LOG("to " << addr.str() << " (" << data.size() << "): \"" << data << "\"");
        usleep(500000);
    }

    TLOG("Stopped");
    sock.close();
    return 0;
}

int udpserver_test(){
    LOG("=== UDP Socket Server Test...");
    ZError::registerInterruptHandler(stopHandler);
    ZError::registerSignalHandler(ZError::terminate, stopHandler);

    ZSocket sock(ZSocket::udp);
    sock.allowRebind(true);
    if(!sock.open(ZAddress(8998))){
        ELOG("Socket Open Fail");
        return 2;
    }

    //sock.setBlocking(false);

    LOG("Listening...");
    //sock.listen(receivedGram);

    while(run){
        ZAddress sender;
        ZBinary data;
        if(!sock.receive(sender, data))
            continue;
        LOG("from " << sender.str() << " (" << data.size() << "): \"" << data << "\"");
    }

    TLOG("Stopped");
    sock.close();
    return 0;
}
