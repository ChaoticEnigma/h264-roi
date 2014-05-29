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

zu64 ZSocket::socket_count = 0;

ZSocket::ZSocket(socket_type type) : _socket(0), _type(type), buffer(nullptr), reuseaddr(false){
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

bool getSocket(int &fd, ZAddress addr){
    fd = ::socket(addr.family(), addr.type(), addr.protocol());
    if(fd <= 0){
        ELOG("ZSocket: failed to create socket " << ZError::getSystemError());
        fd = 0;
        return false;
    }
    return true;
}

bool ZSocket::open(ZAddress addr){
    if(isOpen()){
        ELOG("ZSocket: socket already open");
        return false;
    }
    bool ok = false;
    addr.setType(_type);
    ZArray<ZAddress> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        if(!getSocket(_socket, addrs[i]))
            continue;

        if(reuseaddr){
#if PLATFORM == LINUX
            int opt = 1;
            if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) != 0){
                ELOG("ZSocket: setsockopt error: " << ZError::getSystemError());
                close();
                continue;
            }
#elif PLATFORM == WINDOWS
            if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)TRUE, sizeof(BOOL)) != 0){
                ELOG("ZSocket: setsockopt error: " << ZError::getSystemError());
                close();
                continue;
            }
#endif
        }

        sockaddr_storage addrstorage;
        addrs[i].populate(&addrstorage);
        if(::bind(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            ELOG("ZSocket: bind error " << ZError::getSystemError());
            close();
            continue;
        }
        ok = true;
        _bound = addrs[i];
    }
    if(!ok){
        ELOG("ZSocket: could not create and bind socket on any address");
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

bool ZSocket::send(ZAddress dest, const ZBinary &data){
    if(!isOpen()){
        ELOG("ZConnection: socket is not open");
        return false;
    }
    if(data.size() > ZSOCKET_UDP_MAX)
        return false;

    sockaddr_storage addrstorage;
    dest.setType(_type);
    dest = ZAddress::lookUp(dest)[0];
    dest.populate(&addrstorage);
#if PLATFORM == LINUX
    long sent = ::sendto(_socket, data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#elif PLATFORM == WINDOWS
    long sent = ::sendto(_socket, (const char *)data.raw(), data.size(), 0, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage));
#endif
    if(sent < 0)
        ELOG("ZSocket: sendto error " << ZError::getSystemError());
    return (zu64)sent == data.size();
}

zu32 ZSocket::receive(ZAddress &sender, ZBinary &str){
    if(!isOpen()){
        ELOG("ZConnection: socket is not open");
        return false;
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
    str = ZBinary(buffer, received);
    return received;
}

void ZSocket::receiveFunc(receiveCallback receivedFunc){
    while(true){
        ZAddress sender;
        ZBinary data;
        if(!receive(sender, data))
            continue;
        receivedFunc(this, sender, data);
    }
}

bool ZSocket::connect(ZAddress addr, ZConnection &conn){
    if(isOpen()){
        ELOG("ZSocket: socket already open");
        return false;
    }
    bool ok = false;
    ZAddress end_addr;
    addr.setType(_type);
    ZArray<ZAddress> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        if(!getSocket(_socket, addrs[i]))
            continue;

        sockaddr_storage addrstorage;
        addrs[i].populate(&addrstorage);
        if(::connect(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            ELOG("ZSocket: connect error " << ZError::getSystemError());
            close();
            continue;
        }
        end_addr = ZAddress(&addrstorage);
        ok = true;
    }
    if(!ok){
        ELOG("ZSocket: could not create socket and connect to any address");
        return false;
    }
    conn = ZConnection(_socket, end_addr);
    return true;
}

bool ZSocket::listen(){
    if(::listen(_socket, 20) != 0){
        ELOG("ZSocket: listen error " << ZError::getSystemError());
        return false;
    }
    return true;
}

bool ZSocket::accept(ZConnection &conn){
    sockaddr_storage client_sin;
    socklen_t sin_size = sizeof(client_sin);
    int client = ::accept(_socket, (struct sockaddr *)&client_sin, &sin_size);
    if(client <= 0){
        ELOG("ZSocket: failed to accept socket");
        return false;
    }

    ZAddress client_addr(&client_sin);
    conn = ZConnection(client, client_addr);
    return true;

//    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
//    printf("server: got connection from %s\n", s);

//    if (!fork()) { // this is the child process
//        close(sockfd); // child doesn't need the listener
//        if (send(new_fd, "Hello, world!", 13, 0) == -1)
//            perror("send");
//        close(new_fd);
//        exit(0);
//    }
//    close(new_fd);  // parent doesn't need this
}

bool ZSocket::setBlocking(bool set){
    if(!isOpen())
        return false;
#if PLATFORM == LINUX

    int flags = fcntl(_socket, F_GETFL, 0);
    if(flags < 0){
        ELOG("ZSocket: failed to get socket flags error: " << ZError::getSystemError());
        return false;
    }
    flags = set ? (flags &~ O_NONBLOCK) : (flags | O_NONBLOCK);
    if(fcntl(_socket, F_SETFL, flags) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " << ZError::getSystemError());
        return false;
    }

#elif PLATFORM == WINDOWS

    DWORD opt = set ? 0 : 1;
    if(ioctlsocket(_socket, FIONBIO, &opt) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " << ZError::getSystemError());
        return false;
    }

#endif
    return true;
}
void ZSocket::allowRebind(bool set){
    reuseaddr = set;
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
