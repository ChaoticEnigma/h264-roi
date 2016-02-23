/*******************************************************************************
**                                  LibChaos                                  **
**                                 zsocket.cpp                                **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zsocket.h"
#include "zlog.h"
#include "zexception.h"
#include "zerror.h"

#ifdef ZSOCKET_WINAPI
    #include <winsock2.h>
    #include <windows.h>
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #if PLATFORM == MACOSX
        #include <sys/uio.h>
        #include <unistd.h>
    #endif
#endif

namespace LibChaos {

zu32 ZSocket::socket_count = 0;

ZSocket::ZSocket(socket_type type) : _socket(0), _type(type), buffer(nullptr), buffersize(1024), reuseaddr(false){

}

ZSocket::ZSocket(socket_type type, zsocktype fd) : ZSocket(type){
    _socket = fd;
}

ZSocket::~ZSocket(){
    delete buffer;
}

bool ZSocket::getSocket(zsocktype &fd, int type, int proto){
    //DLOG("ZSocket::getSocket " + addr.familyStr() + " " + addr.typeStr() + " " + addr.protocolStr());
    DLOG("ZSocket::getSocket " << type + " " + proto);
    fd = ::socket(AF_INET, _type, proto);
    if(fd <= 0){
        ELOG("ZSocket: failed to create socket: " + ZError::getSystemError());
        fd = 0;
        return false;
    }
    return true;
}

bool ZSocket::open(){
    // Initialize sockets if needed
    if(socket_count <= 0)
        InitializeSockets();
    ++socket_count;

    if(!getSocket(_socket, _type, 0)){
        ELOG("ZSocket::open Failed to create socket");
        return false;
    }
    return true;
}

bool ZSocket::bind(ZAddress addr){
    if(!isOpen()){
        ELOG("ZSocket: socket not open");
        return false;
    }

    bool ok = false;
    //addr.setType(_type);

    // Loop up address/hostname
    ZArray<SockAddr> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        // Try addresses
        DLOG("ZSocket::open Trying " + addrs[i].addr.debugStr());

        // Set SO_REUSEADDR option if requested
        if(reuseaddr){
            setSocketOption(SocketOptions::reuseaddr, 1);
        }

        // Try to bind socket
        sockaddr_storage addrstorage;
        addrs[i].addr.populate(&addrstorage);
        if(::bind(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            ELOG("ZSocket: failed to bind " +  addrs[i].addr.debugStr() + " - error " + ZError::getSystemError());
            close();
            continue;
        }
        ok = true;
        _bound = addrs[i].addr;

        LOG("Bound socket " << _socket);
    }
    // Could not bind socket with any available addres
    if(!ok){
        ELOG("ZSocket: could not create and bind socket on any address");
        return false;
    }
    return true;
}

void ZSocket::close(){
    if(isOpen()){
        LOG("Closing socket " << _socket);
#ifdef ZSOCKET_WINAPI
        ::closesocket(_socket);
#else
        ::close(_socket);
#endif
        _socket = 0;
    }

    --socket_count;
    if(socket_count <= 0)
        ShutdownSockets();
}

bool ZSocket::isOpen() const {
    return _socket != 0;
}

bool ZSocket::send(ZAddress dest, const ZBinary &data){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
        return false;
    }
    if(data.size() > ZSOCKET_UDP_MAX)
        return false;

    sockaddr_storage addrstorage;
    //dest.setType(_type);
    dest = ZAddress::lookUp(dest)[0].addr;
    dest.populate(&addrstorage);

#if COMPILER == MSVC
    long sent = ::sendto(_socket, (const char *)data.raw(), (int)data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#elif COMPILER == MINGW
    long sent = ::sendto(_socket, (const char *)data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#else
    long sent = ::sendto(_socket, data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#endif

    if(sent < 0)
        ELOG("ZSocket::send sendto error " + ZError::getSystemError());
    return (zu64)sent == data.size();
}

zu64 ZSocket::receive(ZAddress &sender, ZBinary &data){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
        return 0;
    }
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_UDP_BUFFER];
    //memset(buffer, 0, ZSOCKET_BUFFER);
    sockaddr_storage from;
    socklen_t fromLength = sizeof(from);
#ifdef ZSOCKET_WINAPI
    long received = ::recvfrom(_socket, (char *)buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr*)&from, &fromLength);
#else
    long received = ::recvfrom(_socket, buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr*)&from, &fromLength);
#endif
    if(received < 0){
        ELOG("ZSocket:receive error - " + ZError::getSystemError());
        return 0;
    } else if(received == 0){
        return 0;
    } else {
        sender = ZAddress(&from);
        //sender = ZAddress(ntohl(from.sin_addr.s_addr), ntohs(from.sin_port));
        data = ZBinary(buffer, (zu64)received);
        return (zu64)received;
    }
}

bool ZSocket::connect(ZAddress addr, zsocktype &connfd, ZAddress &connaddr){
    if(isOpen()){
        ELOG("ZSocket: socket already open");
        return false;
    }
    bool ok = false;
    ZAddress end_addr;
    //addr.setType(_type);
    ZArray<SockAddr> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        if(!getSocket(_socket, addrs[i].type, addrs[i].proto))
            continue;
        LOG("Got socket for " << addrs[i].addr.debugStr());
        sockaddr_storage addrstorage;
        addrs[i].addr.populate(&addrstorage);
        if(::connect(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            ELOG("ZSocket: connect error " + ZError::getSystemError());
            close();
            continue;
        }
        end_addr = addrs[i].addr;
        ok = true;
    }
    if(!ok){
        ELOG("ZSocket: could not create socket and connect to any address");
        return false;
    }
    connfd = _socket;
    connaddr = end_addr;
    return true;
}

bool ZSocket::listen(){
    if(::listen(_socket, 20) != 0){
        ELOG("ZSocket: listen error " + ZError::getSystemError());
        return false;
    }
    return true;
}

bool ZSocket::accept(zsocktype &connfd, ZAddress &connaddr){
    sockaddr_storage client_sin;
    socklen_t sin_size = sizeof(client_sin);
    zsocktype client = ::accept(_socket, (struct sockaddr *)&client_sin, &sin_size);
    if(client <= 0){
        ELOG("ZSocket: failed to accept socket: " + ZError::getSystemError());
        return false;
    }

    LOG("Accepted socket " << client << " on " << _socket);

    ZAddress client_addr(&client_sin);
    connfd = client;
    connaddr = client_addr;
    return true;
}

zu64 ZSocket::read(ZBinary &data){
    if(!isOpen()){
        ELOG("ZConnection: socket is not open");
        return 0;
    }

    if(!data.size()){
        return 0;
    }

#if COMPILER == MSVC
    long bytes = ::recv(_socket, (char *)data.raw(), (int)data.size(), 0);
#elif COMPILER == MINGW
    long bytes = ::recv(_socket, (char *)data.raw(), data.size(), 0);
#else
    long bytes = ::read(_socket, data.raw(), data.size());
#endif

    if(bytes <= -1){
        ELOG("ZSocket: read error: " + ZError::getSystemError());
        return 0;
    }

    return (zu64)bytes;
}

bool ZSocket::write(const ZBinary &data){
    if(!isOpen()){
        ELOG("ZConnection: socket is not open");
        return false;
    }
    if(data.size() <= 0){
        ELOG("ZConnection: empty write data");
        return false;
    }

#if COMPILER == MSVC
    long bytes = ::send(_socket, (const char *)data.raw(), (int)data.size(), 0);
#elif COMPILER == MINGW
    long bytes = ::send(_socket, (const char *)data.raw(), data.size(), 0);
#else
    long bytes = ::write(_socket, data.raw(), data.size());
#endif

    if(bytes <= 0){
        ELOG("ZSocket: write error: " + ZError::getSystemError());
        return false;
    }
    return true;
}

bool ZSocket::setSocketOption(SocketOptions::socketoption option, int value){
    if(!isOpen())
        return false;

    int sockoption = 0;
    int yes = 1;
    void *optptr = nullptr;
    socklen_t optptrsize = 0;

    switch(option){
    case SocketOptions::reuseaddr: {
        sockoption = SO_REUSEADDR;
#ifdef ZSOCKET_WINAPI
        optptr = (char *)!!(value);
        optptrsize = sizeof(BOOL);
#else
        optptr = &yes;
        optptrsize = sizeof(int);
#endif
        break;
    }

    case SocketOptions::recvtimeout: {
        sockoption = SO_RCVTIMEO;
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = value;
#ifdef ZSOCKET_WINAPI
        throw ZException("FIX ME ON WINDOWS");
#else
        optptr = &tv;
        optptrsize = sizeof(timeval);
#endif
        break;
    }

    default:
        return false;
    }

#ifdef ZSOCKET_WINAPI
    if(optptr != nullptr && setsockopt(_socket, SOL_SOCKET, sockoption, (char *)optptr, optptrsize) != 0){
#else
    if(optptr != nullptr && setsockopt(_socket, SOL_SOCKET, sockoption, optptr, optptrsize) != 0){
#endif
        ELOG("ZSocket: setsockopt error: " + ZError::getSystemError());
        return false;
    }
    return true;
}

bool ZSocket::setBlocking(bool set){
    if(!isOpen())
        return false;
#ifdef ZSOCKET_WINAPI
    DWORD opt = set ? 0 : 1;
    if(ioctlsocket(_socket, (long)FIONBIO, &opt) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " + ZError::getSystemError());
        return false;
    }
#else
    int flags = fcntl(_socket, F_GETFL, 0);
    if(flags < 0){
        ELOG("ZSocket: failed to get socket flags error: " + ZError::getSystemError());
        return false;
    }
    flags = set ? (flags &~ O_NONBLOCK) : (flags | O_NONBLOCK);
    if(fcntl(_socket, F_SETFL, flags) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " + ZError::getSystemError());
        return false;
    }
#endif
    return true;
}
void ZSocket::allowRebind(bool set){
    if(isOpen())
        setSocketOption(SocketOptions::reuseaddr, (int)set);
    reuseaddr = set;
}

void ZSocket::setBufferSize(zu32 size){
    if(buffersize != size)
        delete[] buffer;
    buffersize = size;
}

bool ZSocket::InitializeSockets(){
#ifdef ZSOCKET_WINAPI
    WSADATA WsaData;
    if(WSAStartup(MAKEWORD(2,2), &WsaData) != 0){
        ELOG("ZSocket: WSAStartup Failed to Initialize Sockets");
        return false;
    }
#endif
    return true;
}

void ZSocket::ShutdownSockets(){
#ifdef ZSOCKET_WINAPI
    WSACleanup();
#endif
}

}
