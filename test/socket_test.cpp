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

    ZSocket sock;
    if(!sock.open(ZAddress(0, 8998))){
        ELOG("Socket Open Fail");
        return 2;
    }
    LOG("Sending...");

    zu64 count = 0;
    //ZAddress addr(127,0,0,1, 8998);
    ZAddress addr(192,168,1,38, 8998);
    //ZAddress addr(192,168,1,89, 8998);

    while(run){
        ZString str = "hello world out there! ";
        str << ZString::ItoS(count);
        ZBinary data((unsigned char *)str.cc(), str.size());
        sock.send(addr, data);
        LOG("to " << addr.fullStr() << " (" << data.size() << "): \"" << data << "\"");
        ++count;
        usleep(500000);
    }

    TLOG("Stopped");
    sock.close();
    return 0;
}

void *srvThread(void *zarg){
    LOG("Sending...");

    zu64 count = 0;
    //ZAddress addr(127,0,0,1, 8998);
    ZAddress addr(192,168,1,38, 8998);
    //ZAddress addr(192,168,1,89, 8998);

    while(run){
        ZString str = "hello world out there! ";
        str << ZString::ItoS(count);
        ZBinary data((unsigned char *)str.cc(), str.size());
        ((ZSocket*)(((ZThreadArg*)zarg)->arg))->send(addr, data);
        LOG("to " << addr.fullStr() << " (" << data.size() << "): \"" << data << "\"");
        ++count;
        usleep(500000);
    }
    return NULL;
}

int udpserver_test(){
    LOG("=== UDP Socket Server Test...");
    ZError::registerInterruptHandler(stopHandler);
    ZError::registerSignalHandler(ZError::terminate, stopHandler);

    ZSocket sock;
    if(!sock.open(ZAddress(0, 8998))){
        ELOG("Socket Open Fail");
        return 2;
    }

    ZThread thread;
    thread.run(srvThread, (void *)&sock);

    LOG("Listening...");
    //sock.listen(receivedGram);

    while(run){
        ZAddress sender;
        ZBinary data;
        if(!sock.receive(sender, data))
            continue;
        LOG("from " << sender.fullStr() << " (" << data.size() << "): \"" << data << "\"");
    }

    TLOG("Stopped");
    sock.close();
    return 0;
}
