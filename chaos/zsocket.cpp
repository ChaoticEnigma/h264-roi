#include "zsocket.h"
#include "zlog.h"

#if PLATFORM == WINDOWS
    #include <winsock2.h>
    #include <windows.h>
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cstring>
#endif

#include <string.h>
#include <sys/types.h>

#define ZSOCKET_BUFFER 1024 * 64
#define ZSOCKET_MAX 1024 * 64 - 9

#if PLATFORM == WINDOWS
    typedef int socklen_t;
#endif

namespace LibChaos {

zu64 ZSocket::socket_count = 0;

ZSocket::ZSocket(socket_type type) : _socket(0), _type(type), buffer(nullptr){
    if(socket_count <= 0)
        InitializeSockets();
    ++socket_count;
}

ZSocket::~ZSocket(){
    close();
    delete buffer;
    --socket_count;
    if(socket_count <= 0)
        ShutdownSockets();
}

bool ZSocket::open(ZAddress addr){
    if(isOpen())
        return false;

    if(_type == udp)
        _socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    else if(_type == tcp)
        _socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    else
        _socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(_socket <= 0){
        ELOG("ZSocket: failed to create socket");
        _socket = 0;
        return false;
    }

    sockaddr_storage addrstorage;
    if(addr.isName())
        addr = addr.doLookup();
    addr.populate(&addrstorage);
    if(::bind(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
        ELOG("ZSocket: bind error " << errno << ": " << strerror(errno));
        close();
        return false;
    }
    return true;
}

void ZSocket::close(){
    if(isOpen()){
#if PLATFORM == LINUX
        ::close(_socket);
#elif PLATFORM == WINDOWS
        ::closesocket(_socket);
#endif
        _socket = 0;
    }
}

bool ZSocket::isOpen() const {
    return _socket != 0;
}

bool ZSocket::send(ZAddress destination, const ZBinary &data){
    if(!isOpen())
        return false;

    sockaddr_storage addrstorage;
    if(destination.isName())
        destination = destination.doLookup();
    destination.populate(&addrstorage);
#if PLATFORM == LINUX
    long sent = ::sendto(_socket, data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#elif PLATFORM == WINDOWS
    long sent = ::sendto(_socket, (const char *)data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#endif
    return (zu64)sent == data.size();
}

zu32 ZSocket::receive(ZAddress &sender, ZBinary &str){
    if(!isOpen())
        return false;
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_BUFFER];
    //memset(buffer, 0, ZSOCKET_BUFFER);
    sockaddr_in from;
    socklen_t fromLength = sizeof(from);
#if PLATFORM == LINUX
    long received = ::recvfrom(_socket, buffer, ZSOCKET_BUFFER, 0, (sockaddr*)&from, &fromLength);
#elif PLATFORM == WINDOWS
    long received = ::recvfrom(_socket, (char *)buffer, ZSOCKET_BUFFER, 0, (sockaddr*)&from, &fromLength);
#endif
    if(received <= 0)
        return 0;
    sender = ZAddress(ntohl(from.sin_addr.s_addr), ntohs(from.sin_port));
    str = ZBinary(buffer, received);
    return received;
}

void ZSocket::listen(receiveCallback receivedFunc){
    while(true){
        ZAddress sender;
        ZBinary data;
        if(!receive(sender, data))
            continue;
        receivedFunc(this, sender, data);
    }
}

bool ZSocket::openStream(ZAddress address){

    return true;
}

bool ZSocket::setBlocking(bool set){
    if(!isOpen())
        return false;
#if PLATFORM == LINUX

    int flags = fcntl(_socket, F_GETFL, 0);
    if(flags < 0){
        ELOG("ZSocket: failed to get socket flags error: " << errno << " " << strerror(errno));
        return false;
    }
    flags = set ? (flags &~ O_NONBLOCK) : (flags | O_NONBLOCK);
    if(fcntl(_socket, F_SETFL, flags) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " << errno << " " << strerror(errno));
        return false;
    }

#elif PLATFORM == WINDOWS

    DWORD opt = set ? 0 : 1;
    if(ioctlsocket(_socket, FIONBIO, &opt) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " << errno << " " << strerror(errno));
        return false;
    }

#endif
    return true;
}
bool ZSocket::allowRebind(bool set){
#if PLATFORM == LINUX
    int opt = set ? 1 : 0;
    if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) != 0){
        ELOG("ZSocket: setsockopt error: " << errno << " " << strerror(errno));
        return false;
    }
#elif PLATFORM == WINDOWS
    const char *opt = set ? (const char *)TRUE : (const char *)FALSE;
    if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, opt, sizeof(BOOL)) != 0){
        ELOG("ZSocket: setsockopt error: " << errno << " " << strerror(errno));
        return false;
    }
#endif
    return true;
}

bool ZSocket::InitializeSockets(){
#if PLATFORM == WINDOWS
    WSADATA WsaData;
    if(WSAStartup(MAKEWORD(2,2), &WsaData) != 0){
        ELOG("ZSocket: WSAStartup Failed to Initialize Sockets");
        return false;
    }
#endif
    return true;
}

void ZSocket::ShutdownSockets(){
#if PLATFORM == WINDOWS
    WSACleanup();
#endif
}

}
