/*******************************************************************************
**                                  LibChaos                                  **
**                                 zsocket.cpp                                **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zsocket.h"
#include "zaddress.h"
#include "zlog.h"
#include "zexception.h"
#include "zerror.h"

#ifdef ZSOCKET_WINAPI
    #include <winsock2.h>
    #include <ws2tcpip.h>
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

bool ZSocket::open(int family, int type, int proto){
    // Initialize sockets if needed
    if(socket_count <= 0)
        InitializeSockets();
    ++socket_count;

    _family = (ZAddress::address_family)family;
    _type = (ZSocket::socket_type)type;
    _socket = ::socket(_family, _type, proto);
    if(_socket <= 0){
        ELOG("ZSocket::open failed to create socket: " + ZError::getSystemError());
        _socket = 0;
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

bool ZSocket::bind(ZAddress addr){
    // Loop up address/hostname
    bool ok = false;
    ZList<SockAddr> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        SockAddr saddr = addrs.front();

        // Try address
        DLOG("ZSocket::open Trying " + saddr.addr.debugStr());
        if(!open(saddr.addr.family(), saddr.type, saddr.proto)){
            addrs.rotate();
            continue;
        }

        // Set SO_REUSEADDR option if requested
        if(reuseaddr){
            setSocketOption(SocketOptions::OPT_REUSEADDR, 1);
        }

        // Try to bind socket
        sockaddr_storage addrstorage;
        addrs.front().addr.populate(&addrstorage);
        if(::bind(_socket, (const sockaddr *)&addrstorage, getSockAddrLen(addrs.front().addr.family())) != 0){
            ELOG("ZSocket: failed to bind " +  addrs.front().addr.debugStr() + " - error " + ZError::getSystemError());
            close();
            addrs.rotate();
            continue;
        }
        ok = true;
        _bound = addrs.front().addr;
        DLOG("Bound socket " << _socket);
        break;
    }

    // Could not bind socket with any available addres
    if(!ok){
        ELOG("ZSocket: could not create and bind socket on any address");
        return false;
    }
    return true;
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
    dest = ZAddress::lookUp(dest).front().addr;
    dest.populate(&addrstorage);

#if COMPILER == MSVC
    long sent = ::sendto(_socket, (const char *)data.raw(), (int)data.size(), 0, (const sockaddr *)&addrstorage, getSockAddrLen(dest.family()));
#elif COMPILER == MINGW
    long sent = ::sendto(_socket, (const char *)data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, getSockAddrLen(dest.family()));
#else
    long sent = ::sendto(_socket, data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, getSockAddrLen(dest.family()));
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
    socklen_t fromlen = sizeof(from);
#ifdef ZSOCKET_WINAPI
    long received = ::recvfrom(_socket, (char *)buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr*)&from, &fromlen);
#else
    long received = ::recvfrom(_socket, buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr *)&from, &fromlen);
#endif
    if(received < 0){
        ELOG("ZSocket:receive error - " + ZError::getSystemError());
        return 0;
    } else if(received == 0){
        return 0;
    } else {
        sender = ZAddress(&from, fromlen);
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
    ZList<SockAddr> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        if(!open(addrs.front().addr.family(), addrs.front().type, addrs.front().proto)){
            addrs.rotate();
            continue;
        }
        LOG("Got socket for " << addrs.front().addr.debugStr());
        ZAddress caddr = addrs.front().addr;
        sockaddr_storage addrstorage;
        caddr.populate(&addrstorage);
        if(::connect(_socket, (const sockaddr *)&addrstorage, getSockAddrLen(caddr.family())) != 0){
            ELOG("ZSocket: connect error " + ZError::getSystemError());
            close();
            addrs.rotate();
            continue;
        }
        ok = true;
        break;
    }
    if(!ok){
        ELOG("ZSocket: could not create socket and connect to any address");
        return false;
    }
    connfd = _socket;
    connaddr = addrs.front().addr;
    return true;
}

bool ZSocket::listen(){
    if(::listen(_socket, SOMAXCONN) != 0){
        ELOG("ZSocket: listen error " + ZError::getSystemError());
        return false;
    }
    return true;
}

bool ZSocket::accept(zsocktype &connfd, ZAddress &connaddr){
    sockaddr_storage from;
    socklen_t fromlen = sizeof(from);
    zsocktype client = ::accept(_socket, (struct sockaddr *)&from, &fromlen);
    if(client <= 0){
        ELOG("ZSocket: failed to accept socket: " + ZError::getSystemError());
        return false;
    }

    DLOG("Accepted socket " << client << " on " << _socket);

    connfd = client;
    ZAddress client_addr(&from, fromlen);
    connaddr = client_addr;
    return true;
}

zu64 ZSocket::read(ZBinary &data){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
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
    case SocketOptions::OPT_REUSEADDR: {
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

    case SocketOptions::OPT_RECVTIMEOUT: {
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
        setSocketOption(SocketOptions::OPT_REUSEADDR, (int)set);
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

socklen_t ZSocket::getSockAddrLen(int family){
    if(family == AF_INET)
        return sizeof(sockaddr_in);
    else if(family == AF_INET6)
        return sizeof(sockaddr_in6);
    else
        return 0;
}

}
