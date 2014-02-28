#include "zconnection.h"
#include "zlog.h"
#include "zerror.h"

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

ZConnection::ZConnection() : _socket(0), buffer(NULL){

}

ZConnection::ZConnection(int fd, ZAddress addr) : _socket(fd), _addr(addr), buffer(NULL){

}

ZConnection::~ZConnection(){
    close();
    delete buffer;
}

void ZConnection::close(){
    if(isOpen()){
#if PLATFORM == LINUX
        ::close(_socket);
#elif PLATFORM == WINDOWS
        ::closesocket(_socket);
#endif
        _socket = 0;
    }
}

bool ZConnection::isOpen() const {
    return _socket != 0;
}

zu64 ZConnection::read(ZBinary &str){
    if(!isOpen()){
        ELOG("ZConnection: socket is not open");
        return 0;
    }
        long bytes;
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_TCP_BUFFER];

#if PLATFORM == LINUX
    bytes = ::recv(_socket, buffer, ZSOCKET_TCP_BUFFER, 0);
#elif PLATFORM == WINDOWS
    bytes = ::recv(_socket, (char *)buffer, ZSOCKET_TCP_BUFFER, 0);
#endif
    if(bytes <= -1){
        ELOG("ZSocket: read error: " << ZError::getError());
        return 0;
    }
    str = ZBinary(buffer, bytes);
    return bytes;
}

bool ZConnection::write(const ZBinary &data){
    if(!isOpen()){
        ELOG("ZConnection: socket is not open");
        return false;
    }
    if(data.size() <= 0){
        ELOG("ZConnection: empty write data");
        return false;
    }
    long bytes;
#if PLATFORM == LINUX
    bytes = ::send(_socket, data.raw(), data.size(), 0);
#elif PLATFORM == WINDOWS
    bytes = ::send(_socket, (const char *)data.raw(), data.size(), 0);
#endif
    if(bytes <= 0){
        ELOG("ZSocket: write error: " << ZError::getError());
        return false;
    }
    return true;
}

ZAddress ZConnection::other(){
    return _addr;
}

}
