#include "test.h"
#include "zsocket.h"
#include "zerror.h"

static bool run = true;

void stopHandler2(ZError::zerror_signal sig){
    run = false;
}

int tcp_test(){
    LOG("=== TCP Socket Test...");
    ZError::registerInterruptHandler(stopHandler2);
    ZError::registerSignalHandler(ZError::terminate, stopHandler2);

    ZSocket sock(ZSocket::stream);

    //ZAddress addr("::1", 8998);
    ZAddress addr("127.0.0.1", 8998);
    //ZAddress addr("192.168.1.71", 8998);
    //ZAddress addr("192.168.1.89", 8998);

    ZConnection conn;
    LOG("Connecting to " << addr.str());
    if(!sock.connect(addr, conn)){
        ELOG("Socket Connect Fail");
        return 3;
    }
    LOG("connected " << conn.other().str());

    ZString str = "hi connection!";
    ZBinary snddata((unsigned char *)str.cc(), str.size());
    conn.write(snddata);
    LOG("write (" << snddata.size() << "): \"" << snddata << "\"");

    ZBinary data;
    conn.read(data);
    LOG("read (" << data.size() << "): \"" << data << "\"");


    return 0;
}
int tcpserver_test(){
    LOG("=== TCP Socket Server Test...");
    ZError::registerInterruptHandler(stopHandler2);
    ZError::registerSignalHandler(ZError::terminate, stopHandler2);

    ZSocket sock(ZSocket::stream);
    if(!sock.open(ZAddress(8998))){
        ELOG("Socket Open Fail");
        return 2;
    }
    if(!sock.listen()){
        ELOG("Socket Listen Fail");
        return 3;
    }

    LOG("Listening...");

    while(run){
        ZConnection client;
        if(!sock.accept(client))
            continue;
        LOG("accept " << client.other().debugStr());

        ZBinary data;
        client.read(data);
        LOG("read (" << data.size() << "): \"" << data << "\"");

        ZString str = "hello back there!";
        ZBinary snddata((unsigned char *)str.cc(), str.size());
        client.write(snddata);
        LOG("write (" << snddata.size() << "): \"" << snddata << "\"");
    }

    return 0;
}
