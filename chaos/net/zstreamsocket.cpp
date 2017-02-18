/*******************************************************************************
**                                  LibChaos                                  **
**                              zstreamsocket.cpp                             **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zstreamsocket.h"

namespace LibChaos {

ZStreamSocket::ZStreamSocket() : ZSocket(ZSocket::STREAM){

}

bool ZStreamSocket::connect(ZAddress addr, ZConnection &conn){
    zsocktype connfd;
    ZAddress connaddr;
    bool ret = ZSocket::connect(addr, connfd, connaddr);
    conn = ZConnection(connfd, connaddr);
    return ret;
}

bool ZStreamSocket::listen(ZAddress bindaddr){
    if(!ZSocket::bind(bindaddr))
        return false;
    if(!ZSocket::listen())
        return false;
    return true;
}

ZSocket::socketerror ZStreamSocket::accept(ZPointer<ZConnection> &conn){
    zsocktype connfd;
    ZAddress connaddr;
    socketerror ret = ZSocket::accept(connfd, connaddr);
    if(ret == OK){
        conn = new ZConnection(connfd, connaddr);
    } else {
        conn = nullptr;
    }
    return ret;
}

}
