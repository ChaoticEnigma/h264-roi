/*******************************************************************************
**                                  LibChaos                                  **
**                               zconnection.cpp                              **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zconnection.h"
#include "zlog.h"

namespace LibChaos {

ZConnection::ZConnection() : ZSocket(STREAM), buffer(nullptr){

}

ZConnection::ZConnection(zsocktype fd, ZAddress addr) : ZSocket(STREAM, fd), _peeraddr(addr), buffer(nullptr){

}

ZConnection::~ZConnection(){
    // Close the socket
    close();
    delete buffer;
}

ZAddress ZConnection::peer(){
    return _peeraddr;
}

}
