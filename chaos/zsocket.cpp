#include "zsocket.h"
#include "zlog.h"

#if PLATFORM == WINDOWS
    #include <winsock2.h>
    #include <windows.h>
    #include <winerror.h>
    typedef int socklen_t;
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
#endif

#define ZSOCKET_UDP_BUFFER  1024 * 64
#define ZSOCKET_UDP_MAX     1024 * 64 - 9

#define ZSOCKET_TCP_BUFFER  1024

namespace LibChaos {

zu64 ZSocket::socket_count = 0;

ZString getError(){
#if PLATFORM == LINUX
    int err = errno;
    return ZString() << err << ": " << strerror(err);
#elif  PLATFORM == WINDOWS
    int err = WSAGetLastError();
    char *s = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), s, 0, NULL);
    LocalFree(s);
    return ZString() << err << ": " << s;
#endif
}

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

bool ZSocket::open(ZAddress addr){
    if(isOpen()){
        ELOG("ZSocket: socket already open");
        return false;
    }
    bool ok = false;
    addr.setType(_type);
    ZArray<ZAddress> addrs = ZAddress::lookUp(addr);
    for(zu64 i = 0; i < addrs.size(); ++i){
        _socket = ::socket(addrs[i].family(), addrs[i].type(), addrs[i].protocol());
        if(_socket <= 0){
            ELOG("ZSocket: failed to create socket " << getError());
            _socket = 0;
            continue;
        }

        if(reuseaddr){
#if PLATFORM == LINUX
            int opt = 1;
            if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) != 0){
                ELOG("ZSocket: setsockopt error: " << getError());
                close();
                continue;
            }
#elif PLATFORM == WINDOWS
            if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)TRUE, sizeof(BOOL)) != 0){
                ELOG("ZSocket: setsockopt error: " << getError());
                close();
                continue;
            }
#endif
        }

        sockaddr_storage addrstorage;
        addrs[i].populate(&addrstorage);
        if(::bind(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            ELOG("ZSocket: bind error " << getError());
            close();
            continue;
        }
        ok = true;
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
    if(!isOpen())
        return false;
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
    return (zu64)sent == data.size();
}

zu32 ZSocket::receive(ZAddress &sender, ZBinary &str){
    if(!isOpen())
        return false;
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
        _socket = ::socket(addrs[i].family(), addrs[i].type(), addrs[i].protocol());
        if(_socket <= 0){
            ELOG("ZSocket: failed to create socket " << getError());
            _socket = 0;
            continue;
        }

        sockaddr_storage addrstorage;
        addrs[i].populate(&addrstorage);
        if(::connect(_socket, (const sockaddr *)&addrstorage, sizeof(sockaddr_storage)) != 0){
            ELOG("ZSocket: connect error " << getError());
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
        ELOG("ZSocket: listen error " << getError());
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
        ELOG("ZSocket: failed to get socket flags error: " << getError());
        return false;
    }
    flags = set ? (flags &~ O_NONBLOCK) : (flags | O_NONBLOCK);
    if(fcntl(_socket, F_SETFL, flags) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " << getError());
        return false;
    }

#elif PLATFORM == WINDOWS

    DWORD opt = set ? 0 : 1;
    if(ioctlsocket(_socket, FIONBIO, &opt) != 0){
        ELOG("ZSocket: failed to set non-blocking socket error: " << getError());
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

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZConnection::ZConnection() : _socket(0), buffer(NULL){

}

ZConnection::ZConnection(int fd, ZAddress addr) : _socket(fd), _addr(addr), buffer(NULL){

}

ZConnection::~ZConnection(){
#if PLATFORM == LINUX
        ::close(_socket);
#elif PLATFORM == WINDOWS
        ::closesocket(_socket);
#endif
    delete buffer;
}

zu64 ZConnection::read(ZBinary &str){
    long bytes;
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_TCP_BUFFER];

#if PLATFORM == LINUX
    if((bytes = ::recv(_socket, buffer, ZSOCKET_TCP_BUFFER, 0)) != 0) {
#elif PLATFORM == WINDOWS
    if((bytes = ::recv(_socket, (char *)buffer, ZSOCKET_TCP_BUFFER, 0)) != 0) {
#endif
        ELOG("ZSocket: recv error: " << getError());
        return 0;
    }
    str = ZBinary(buffer, bytes);
    return bytes;
}

bool ZConnection::write(const ZBinary &data){
    long bytes;
#if PLATFORM == LINUX
    if((bytes = ::send(_socket, data.raw(), data.size(), 0)) == -1){
#elif PLATFORM == WINDOWS
    if((bytes = ::send(_socket, (const char *)data.raw(), data.size(), 0)) == -1){
#endif
        ELOG("ZSocket: send error: " << getError());
        return false;
    }
    return true;
}

ZAddress ZConnection::other(){
    return _addr;
}

}
