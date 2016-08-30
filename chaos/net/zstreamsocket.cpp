/*******************************************************************************
**                                  LibChaos                                  **
**                              zstreamsocket.cpp                             **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zstreamsocket.h"

namespace LibChaos {

ZStreamSocket::ZStreamSocket() : ZSocket(ZSocket::STREAM){

}

bool ZStreamSocket::open(){
    return ZSocket::open();
}

void ZStreamSocket::close(){
    ZSocket::close();
}

bool ZStreamSocket::isOpen() const {
    return ZSocket::isOpen();
}

bool ZStreamSocket::bind(ZAddress port){
    return ZSocket::bind(port);
}

bool ZStreamSocket::connect(ZAddress addr, ZConnection &conn){
    zsocktype connfd;
    ZAddress connaddr;
    bool ret = ZSocket::connect(addr, connfd, connaddr);
    conn = ZConnection(connfd, connaddr);
    return ret;
}

bool ZStreamSocket::listen(){
    return ZSocket::listen();
}

ZPointer<ZConnection> ZStreamSocket::accept(){
    zsocktype connfd;
    ZAddress connaddr;
    bool ret = ZSocket::accept(connfd, connaddr);
    if(ret)
        return ZPointer<ZConnection>(new ZConnection(connfd, connaddr));
    else
        return nullptr;
}

}
