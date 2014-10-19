#include "zsocket.h"
#include "zlog.h"
#include "zerror.h"

#if PLATFORM == WINDOWS
    #include <winsock2.h>
    #include <windows.h>
    typedef int socklen_t;
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
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

bool ZSocket::getSocket(zsocktype &fd, ZAddress addr){
    fd = ::socket(addr.family(), addr.type(), addr.protocol());
    if(fd <= 0){
        error = ZError("ZSocket: failed to create socket " + ZError::getSystemError());
        fd = 0;
        return false;
    }
    return true;
}

bool ZSocket::open(ZAddress addr){
    if(socket_count <= 0)
        InitializeSockets();
    ++socket_count;

    if(isOpen()){
        error = ZError("ZSocket: socket already open");
        return false;
    }
    bool ok = false;
    addr.setType(_type);
    ZArray<ZAddress> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        if(!getSocket(_socket, addrs[i]))
            continue;

        if(reuseaddr){
            setSocketOption(SocketOptions::reuseaddr, 1);
        }

        sockaddr_storage addrstorage;
        addrs[i].populate(&addrstorage);
        if(::bind(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            error = ZError("ZSocket: bind error " + ZError::getSystemError());
            close();
            continue;
        }
        ok = true;
        _bound = addrs[i];

        LOG("Bound socket " << _socket);
    }
    if(!ok){
        error = ZError("ZSocket: could not create and bind socket on any address");
        return false;
    }
    return true;
}

void ZSocket::close(){
    if(isOpen()){
        LOG("Closing socket " << _socket);
#if PLATFORM == LINUX
        ::close(_socket);
#elif PLATFORM == WINDOWS
        ::closesocket(_socket);
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
        error = ZError("ZConnection: socket is not open");
        return false;
    }
    if(data.size() > ZSOCKET_UDP_MAX)
        return false;

    sockaddr_storage addrstorage;
    dest.setType(_type);
    dest = ZAddress::lookUp(dest)[0];
    dest.populate(&addrstorage);

    zbyte *ptr = data.storage()->getBlock(0, data.size());

#if PLATFORM == LINUX
    long sent = ::sendto(_socket, ptr, data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#elif PLATFORM == WINDOWS
    long sent = ::sendto(_socket, (const char *)ptr, data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#endif

    data.storage()->freeBlock(ptr);

    if(sent < 0)
        error = ZError("ZSocket: sendto error " + ZError::getSystemError());
    return (zu64)sent == data.size();
}

zu64 ZSocket::receive(ZAddress &sender, ZBinary &str){
    if(!isOpen()){
        ELOG("ZConnection: socket is not open");
        return 0;
    }
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_UDP_BUFFER];
    //memset(buffer, 0, ZSOCKET_BUFFER);
    sockaddr_storage from;
    socklen_t fromLength = sizeof(from);
#if PLATFORM == LINUX
    long received = ::recvfrom(_socket, buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr*)&from, &fromLength);
#elif PLATFORM == WINDOWS
    long received = ::recvfrom(_socket, (char *)buffer, ZSOCKET_UDP_BUFFER, 0, (sockaddr*)&from, &fromLength);
#endif
    if(received <= 0)
        return 0;
    sender = ZAddress(&from);
    //sender = ZAddress(ntohl(from.sin_addr.s_addr), ntohs(from.sin_port));
    str = ZBinary(buffer, (zu64)received);
    return (zu64)received;
}

bool ZSocket::connect(ZAddress addr, zsocktype &connfd, ZAddress &connaddr){
    if(isOpen()){
        error = ZError("ZSocket: socket already open");
        return false;
    }
    bool ok = false;
    ZAddress end_addr;
    addr.setType(_type);
    ZArray<ZAddress> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        if(!getSocket(_socket, addrs[i]))
            continue;
        LOG("Got socket for " << addrs[i].debugStr());
        sockaddr_storage addrstorage;
        addrs[i].populate(&addrstorage);
        if(::connect(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            error = ZError("ZSocket: connect error " + ZError::getSystemError());
            close();
            continue;
        }
        end_addr = addrs[i];
        ok = true;
    }
    if(!ok){
        error = ZError("ZSocket: could not create socket and connect to any address");
        return false;
    }
    connfd = _socket;
    connaddr = end_addr;
    return true;
}

bool ZSocket::listen(){
    if(::listen(_socket, 20) != 0){
        error = ZError("ZSocket: listen error " + ZError::getSystemError());
        return false;
    }
    return true;
}

bool ZSocket::accept(zsocktype &connfd, ZAddress &connaddr){
    sockaddr_storage client_sin;
    socklen_t sin_size = sizeof(client_sin);
    zsocktype client = ::accept(_socket, (struct sockaddr *)&client_sin, &sin_size);
    if(client <= 0){
        error = ZError("ZSocket: failed to accept socket: " + ZError::getSystemError());
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
        error = ZError("ZConnection: socket is not open");
        return 0;
    }

    if(!data.size()){
        return 0;
    }

    zbyte *ptr = data.storage()->getBlock(0, data.size());

#if PLATFORM == LINUX
    long bytes = ::read(_socket, ptr, data.size());
#elif PLATFORM == WINDOWS
    long bytes = ::recv(_socket, (char *)ptr, data.size(), 0);
#endif

    data.storage()->commitBlock(ptr);

    if(bytes <= -1){
        error = ZError("ZSocket: read error: " + ZError::getSystemError());
        return 0;
    }

    return (zu64)bytes;
}

bool ZSocket::write(const ZBinary &data){
    if(!isOpen()){
        error = ZError("ZConnection: socket is not open");
        return false;
    }
    if(data.size() <= 0){
        error = ZError("ZConnection: empty write data");
        return false;
    }

    zbyte *ptr = data.storage()->getBlock(0, data.size());
#if PLATFORM == LINUX
    long bytes = ::write(_socket, ptr, data.size());
#elif PLATFORM == WINDOWS
    long bytes = ::send(_socket, (const char *)ptr, data.size(), 0);
#endif
    data.storage()->freeBlock(ptr);

    if(bytes <= 0){
        error = ZError("ZSocket: write error: " + ZError::getSystemError());
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
#if PLATFORM == LINUX
        optptr = &yes;
        optptrsize = sizeof(int);
#elif PLATFORM == WINDOWS
        optptr = (char *)!!(value);
        optptrsize = sizeof(BOOL);
#endif
        break;
    }

    case SocketOptions::recvtimeout: {
        sockoption = SO_RCVTIMEO;
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = value;
#if PLATFORM == LINUX
        optptr = &tv;
        optptrsize = sizeof(timeval);
#elif PLATFORM == WINDOWS
        throw ZError("FIX ME ON WINDOWS");
#endif
        break;
    }

    default:
        return false;
    }

#if PLATFORM == LINUX
    if(optptr != nullptr && setsockopt(_socket, SOL_SOCKET, sockoption, optptr, optptrsize) != 0){
#elif PLATFORM == WINDOWS
    if(optptr != nullptr && setsockopt(_socket, SOL_SOCKET, sockoption, (char *)optptr, optptrsize) != 0){
#endif
        error = ZError("ZSocket: setsockopt error: " + ZError::getSystemError());
        return false;
    }
    return true;
}

bool ZSocket::setBlocking(bool set){
    if(!isOpen())
        return false;
#if PLATFORM == LINUX

    int flags = fcntl(_socket, F_GETFL, 0);
    if(flags < 0){
        error = ZError("ZSocket: failed to get socket flags error: " + ZError::getSystemError());
        return false;
    }
    flags = set ? (flags &~ O_NONBLOCK) : (flags | O_NONBLOCK);
    if(fcntl(_socket, F_SETFL, flags) != 0){
        error = ZError("ZSocket: failed to set non-blocking socket error: " + ZError::getSystemError());
        return false;
    }

#elif PLATFORM == WINDOWS

    DWORD opt = set ? 0 : 1;
    if(ioctlsocket(_socket, FIONBIO, &opt) != 0){
        error = ZError("ZSocket: failed to set non-blocking socket error: " + ZError::getSystemError());
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
