#include "test.h"
#include "zdatagramsocket.h"
//#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "zerror.h"

static bool run = true;

void stopHandler(ZError::zerror_signal sig){
    run = false;
}

int udp_test(){
    LOG("=== UDP Socket Test...");
    ZError::registerInterruptHandler(stopHandler);
    ZError::registerSignalHandler(ZError::terminate, stopHandler);

    ZDatagramSocket sock;
    if(!sock.open()){
        ELOG("Socket Open Fail");
        return 2;
    }
    LOG("Sending...");

    ZAddress addr("127.0.0.1", 8998);

    ZString dat = "Hello World out There! ";
    zu64 count = 0;

    for(zu64 i = 0; run && i < 5000; ++i){
        ZString str = dat + ZString::ItoS(i);
        ZBinary data((const unsigned char *)str.cc(), str.size());
        if(sock.send(addr, data)){
            LOG("to " << addr.debugStr() << " (" << data.size() << "): \"" << data << "\"");
            ZAddress sender;
            ZBinary recvdata;
            if(sock.receive(sender, recvdata)){
                LOG("from " << sender.str() << " (" << recvdata.size() << "): \"" << recvdata << "\"");
                count++;
            } else {
                continue;
            }
        } else {
            LOG("failed to send to " << addr.debugStr());
        }
        ZThread::usleep(500000);
    }

    TLOG("Sent " << count);
    sock.close();
    return 0;
}

int udpserver_test(){
    LOG("=== UDP Socket Server Test...");
    ZError::registerInterruptHandler(stopHandler);
    ZError::registerSignalHandler(ZError::terminate, stopHandler);

    ZDatagramSocket sock;
    ZAddress bind(8998);
    DLOG(bind.debugStr());
    if(!sock.open()){
        ELOG("Socket Open Fail " + sock.getError().what());
        TASSERT(false);
    }
    if(!sock.bind(bind)){
        ELOG("Socket Bind Fail " + sock.getError().what());
        TASSERT(false);
    }

    //sock.setBlocking(false);

    //int out;
    //int len = sizeof(out);
    //getsockopt(sock.getHandle(), SOL_SOCKET, SO_REUSEADDR, &out, (socklen_t*)&len);
    //LOG(out);

    LOG("Listening...");
    //sock.listen(receivedGram);

    zu64 count = 0;

    while(run){
        ZAddress sender;
        ZBinary data;
        if(sock.receive(sender, data)){
            LOG("from " << sender.str() << " (" << data.size() << "): \"" << data << "\"");
            count++;
            ZAddress addr = sender;
            if(sock.send(addr, data)){
                LOG("to " << addr.debugStr() << " (" << data.size() << "): \"" << data << "\"");
            } else {
                LOG("failed to send to " << addr.str());
            }
        } else {
            LOG("error receiving message: " << ZError::getSystemError());
            continue;
        }
    }

    TLOG("Received " << count);
    sock.close();
    return 0;
}
