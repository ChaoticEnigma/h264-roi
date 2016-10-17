/*******************************************************************************
**                                  LibChaos                                  **
**                                 zsocket.cpp                                **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zsocket.h"
#include "zaddress.h"
#include "zerror.h"
#include "zexception.h"
#include "zlog.h"

#ifdef ZSOCKET_WINAPI
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <errno.h>
    #if PLATFORM == MACOSX
        #include <sys/uio.h>
        #include <unistd.h>
    #endif
#endif


bool InitializeSockets(){
#ifdef ZSOCKET_WINAPI
    WSADATA WsaData;
    if(WSAStartup(MAKEWORD(2,2), &WsaData) != 0){
        ELOG("ZSocket: WSAStartup Failed to Initialize Sockets");
        return false;
    }
#endif
    return true;
}

void ShutdownSockets(){
#ifdef ZSOCKET_WINAPI
    WSACleanup();
#endif
}

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

    // Create socket
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
        DLOG("ZSocket::close socket " << _socket);
#ifdef ZSOCKET_WINAPI
        ::closesocket(_socket);
#else
        ::close(_socket);
#endif
        _socket = 0;

        // Shutdown sockets if needed
        --socket_count;
        if(socket_count <= 0)
            ShutdownSockets();
    }
}

bool ZSocket::isOpen() const {
    return _socket != 0;
}

bool ZSocket::bind(ZAddress addr){
    // Close any existing socket
    close();

    // Look up address/hostname
    bool ok = false;
    ZList<SockAddr> addrs = lookupAddr(addr, _type);
    for(zu64 i = 0; i < addrs.size(); ++i){
        SockAddr saddr = addrs.front();

        // Try address
        DLOG("ZSocket::bind Trying " + saddr.addr.debugStr());
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
        saddr.addr.populate(&addrstorage);
        if(::bind(_socket, (const sockaddr *)&addrstorage, saddr.addr.getSockLen()) != 0){
            ELOG("ZSocket::bind failed to bind " +  saddr.addr.debugStr() + " - error " + ZError::getSystemError());
            close();
            addrs.rotate();
            continue;
        }
        ok = true;
        _bound = saddr.addr;
        DLOG("ZSocket::bind Bound socket " << _socket);
        break;
    }

    // Could not bind socket with any available addres
    if(!ok){
        ELOG("ZSocket::bind could not create and bind socket on any address");
        return false;
    }
    return true;
}

bool ZSocket::connect(ZAddress addr, zsocktype &connfd, ZAddress &connaddr){
    if(isOpen()){
        ELOG("ZSocket::connect socket already open");
        return false;
    }
    bool ok = false;
    ZList<SockAddr> addrs = lookupAddr(addr, _type);
    for(zu64 i = 0; i < addrs.size(); ++i){
        if(!open(addrs.front().addr.family(), addrs.front().type, addrs.front().proto)){
            addrs.rotate();
            continue;
        }
        DLOG("ZSocket::connect Got socket for " << addrs.front().addr.debugStr());
        ZAddress caddr = addrs.front().addr;
        sockaddr_storage addrstorage;
        caddr.populate(&addrstorage);
        if(::connect(_socket, (const sockaddr *)&addrstorage, caddr.getSockLen()) != 0){
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

ZSocket::socketerror ZSocket::accept(zsocktype &connfd, ZAddress &connaddr){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
        return ERR_NOTOPEN;
    }

    sockaddr_storage from;
    socklen_t fromlen = sizeof(from);

    int ret = ::accept(_socket, (struct sockaddr *)&from, &fromlen);

    if(ret < 0){
        int err = ZError::getSocketErrorCode();
        if(err == EAGAIN || err == EWOULDBLOCK){
            return AGAIN;
        }
        ELOG("ZSocket:accept failed: " + ZError::getSystemError());
        return ERR_ACCEPT;
    }

    DLOG("ZSocket::accept socket " << ret << " on " << _socket);

    connfd = ret;
    connaddr = ZAddress(&from, fromlen);
    return OK;
}

ZSocket::socketerror ZSocket::read(ZBinary &data){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
        return ERR_NOTOPEN;
    }
    if(!data.size()){
        return ERR_BUFFER;
    }

#if COMPILER == MSVC
    long ret = ::recv(_socket, (char *)data.raw(), (int)data.size(), 0);
#elif COMPILER == MINGW
    long ret = ::recv(_socket, (char *)data.raw(), data.size(), 0);
#else
    long ret = ::read(_socket, data.raw(), data.size());
#endif

    if(ret == 0){
        return DONE;
    } else if(ret < 0){
        int err = ZError::getSocketErrorCode();
        if(err == EAGAIN || err == EWOULDBLOCK){
            return AGAIN;
        }
        ELOG("ZSocket: read error: " + ZError::getSystemError());
        return ERR_READ;
    }

    data.resize((zu64)ret);
    return OK;
}

ZSocket::socketerror ZSocket::write(const ZBinary &data){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
        return ERR_NOTOPEN;
    }
    if(!data.size()){
        return ERR_BUFFER;
    }

#if COMPILER == MSVC
    long ret = ::send(_socket, (const char *)data.raw(), (int)data.size(), 0);
#elif COMPILER == MINGW
    long ret = ::send(_socket, (const char *)data.raw(), data.size(), 0);
#else
    long ret = ::write(_socket, data.raw(), data.size());
#endif

    if(ret == 0){
        return DONE;
    } else if(ret < 0){
        int err = ZError::getSocketErrorCode();
        if(err == EAGAIN || err == EWOULDBLOCK){
            return AGAIN;
        }
        ELOG("ZSocket: write error: " + ZError::getSystemError());
        return ERR_WRITE;
    }

    return OK;
}

ZSocket::socketerror ZSocket::send(ZAddress dest, const ZBinary &data){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
        return ERR_NOTOPEN;
    }
    if(data.size() == 0 || data.size() > ZSOCKET_UDP_MAX)
        return ERR_BUFFER;

    sockaddr_storage addrstorage;
    //dest.setType(_type);
    //dest = lookupAddr(dest, _type).front().addr;
    dest.populate(&addrstorage);

#if COMPILER == MSVC
    long ret = ::sendto(_socket, (const char *)data.raw(), (int)data.size(), 0, (const sockaddr *)&addrstorage, dest.getSockLen());
#elif COMPILER == MINGW
    long ret = ::sendto(_socket, (const char *)data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, dest.getSockLen());
#else
    long ret = ::sendto(_socket, data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, dest.getSockLen());
#endif

    if(ret < 0){
        int err = ZError::getSocketErrorCode();
        if(err == EAGAIN || err == EWOULDBLOCK){
            return AGAIN;
        }
        ELOG("ZSocket::send sendto error " + ZError::getSystemError());
        return ERR_WRITE;
    }

    return OK;
}

ZSocket::socketerror ZSocket::receive(ZAddress &sender, ZBinary &data){
    if(!isOpen()){
        ELOG("ZSocket: socket is not open");
        return ERR_NOTOPEN;
    }
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_UDP_BUFFER];

    sockaddr_storage from;
    socklen_t fromlen = sizeof(from);

#ifdef ZSOCKET_WINAPI
    long ret = ::recvfrom(_socket, (char *)buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr*)&from, &fromlen);
#else
    long ret = ::recvfrom(_socket, buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr *)&from, &fromlen);
#endif

    if(ret == 0){
        return DONE;
    } else if(ret < 0){
        int err = ZError::getSocketErrorCode();
        if(err == EAGAIN || err == EWOULDBLOCK){
            return AGAIN;
        }
        ELOG("ZSocket:receive error - " + ZError::getSystemError());
        return ERR_READ;
    }

    sender = ZAddress(&from, fromlen);
    data = ZBinary(buffer, (zu64)ret);
    return OK;
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
    // Set or unset nonblocking option
    DWORD opt = set ? 0 : 1;
    if(ioctlsocket(_socket, (long)FIONBIO, &opt) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " + ZError::getSystemError());
        return false;
    }
#else
    // Get socket flags
    int flags = fcntl(_socket, F_GETFL, 0);
    if(flags < 0){
        ELOG("ZSocket: failed to get socket flags error: " + ZError::getSystemError());
        return false;
    }
    // Set or unset nonblocking flag
    flags = (!set) ? (flags | O_NONBLOCK) : (flags & (~O_NONBLOCK));
    // Set socket flags
    if(fcntl(_socket, F_SETFL, flags) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " + ZError::getSystemError());
        return false;
    }
#endif
    return true;
}

void ZSocket::allowRebind(bool set){
    if(isOpen())
        setSocketOption(SocketOptions::OPT_REUSEADDR, set ? 1 : 0);
    reuseaddr = set;
}

void ZSocket::setBufferSize(zu32 size){
    if(buffersize != size)
        delete[] buffer;
    buffersize = size;
}

ZList<ZSocket::SockAddr> ZSocket::lookupAddr(ZAddress addr, int type){
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    //hints.ai_family = AF_UNSPEC; // Redundant
    hints.ai_socktype = type;

    ZString name = addr.str();
    const char *aptr = name.isEmpty() ? NULL : name.cc();

    ZString serv = ZString::ItoS((zu64)addr.port());
    const char *sptr = serv.isEmpty() ? NULL : serv.cc();

    int status;
    addrinfo *result;
    if((status = ::getaddrinfo(aptr, sptr, &hints, &result)) != 0){
        ELOG("ZSocket: getaddrinfo for " << name << " " << (zuint)addr.port() << ": " << status <<": " << gai_strerror(status));
        return ZList<SockAddr>();
    }

    ZList<SockAddr> addrs;
    for(addrinfo *p = result; p != NULL; p = p->ai_next){
        // Other data:
        //      p->ai_family    // Family (AF_INET, AF_INET6)
        //      p->ai_socktype  // Socket type (SOCK_STREAM, SOCK_DGRAM)
        //      p->ai_protocol
        //      p->ai_canonname // Offical service name

        ZAddress newaddr((const sockaddr_storage *)p->ai_addr, p->ai_addrlen);
        newaddr.setPort(addr.port());
        SockAddr sockaddr;
        sockaddr.addr = newaddr;
        sockaddr.type = p->ai_socktype;
        sockaddr.proto = p->ai_protocol;

        addrs.push(sockaddr);
    }

    freeaddrinfo(result); // free the linked list
    return addrs;
}

}
