/*******************************************************************************
**                                  LibChaos                                  **
**                               zconnection.cpp                              **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zconnection.h"
#include "zlog.h"
#include "zexception.h"

#if PLATFORM == WINDOWS
    #include <winsock2.h>
    #include <ws2tcpip.h>
#elif PLATFORM == LINUX
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <string.h>
    #include <unistd.h>
    #include <string.h>
#endif

namespace LibChaos {

ZConnection::ZConnection() : ZSocket(STREAM), buffer(nullptr){

}

ZConnection::ZConnection(zsocktype fd, ZAddress addr) : ZSocket(STREAM, fd), _addr(addr), buffer(nullptr){

}

ZConnection::~ZConnection(){
    //close();
    delete buffer;
}

void ZConnection::close(){
    ZSocket::close();
}

bool ZConnection::isOpen() const {
    return ZSocket::isOpen();
}

zu64 ZConnection::read(ZBinary &data){
    return ZSocket::read(data);
}

bool ZConnection::write(const ZBinary &data){
    return ZSocket::write(data);
}

ZAddress ZConnection::other(){
    return _addr;
}

}
