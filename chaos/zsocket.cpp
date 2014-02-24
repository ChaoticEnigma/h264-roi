#include "zsocket.h"
#include "zlog.h"

#if PLATFORM == WINDOWS
    #include <winsock2.h>
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cstring>
#endif

#define ZSOCKET_BUFFER 1024 * 64
#define ZSOCKET_MAX 1024 * 64 - 9

#if PLATFORM == WINDOWS
    typedef int socklen_t;
#endif

namespace LibChaos {

zu64 ZSocket::socket_count = 0;

ZSocket::ZSocket() : socket(0), buffer(NULL){
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

bool ZSocket::open(zu16 port){
    if(isOpen())
        return false;
    socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket <= 0){
        ELOG("ZSocket: failed to create socket");
        socket = 0;
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short) port);

    if(::bind(socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0){
        ELOG("ZSocket: failed to bind socket");
        close();
        return false;
    }

    #if PLATFORM == LINUX
        int nonBlocking = 1;
        if(fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1){
            ELOG("ZSocket: failed to set non-blocking socket");
            close();
            return false;
        }
    #elif PLATFORM == WINDOWS
        DWORD nonBlocking = 1;
        if(ioctlsocket(socket, FIONBIO, &nonBlocking) != 0){
            ELOG("ZSocket: failed to set non-blocking socket");
            close();
            return false;
        }
    #endif
    return true;
}

void ZSocket::close(){
    if(socket != 0){
#if PLATFORM == LINUX
        ::close(socket);
#elif PLATFORM == WINDOWS
        ::closesocket(socket);
#endif
        socket = 0;
    }
}

bool ZSocket::isOpen() const {
    return socket != 0;
}

bool ZSocket::send(const ZAddress &destination, const ZBinary &data){
    if(socket == 0)
        return false;
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(destination.address());
    address.sin_port = htons(destination.port());
#if PLATFORM == LINUX
    long sent = ::sendto(socket, data.raw(), data.size(), 0, (sockaddr*)&address, sizeof(sockaddr_in));
#elif PLATFORM == WINDOWS
    long sent = ::sendto(socket, (const char *)data.raw(), data.size(), 0, (sockaddr*)&address, sizeof(sockaddr_in));
#endif
    return (zu64)sent == data.size();
}

zu32 ZSocket::receive(ZAddress &sender, ZBinary &str){
    if(socket == 0)
        return false;
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_BUFFER];
    //memset(buffer, 0, ZSOCKET_BUFFER);
    sockaddr_in from;
    socklen_t fromLength = sizeof(from);
#if PLATFORM == LINUX
    long received = ::recvfrom(socket, buffer, ZSOCKET_BUFFER, 0, (sockaddr*)&from, &fromLength);
#elif PLATFORM == WINDOWS
    long received = ::recvfrom(socket, (char *)buffer, ZSOCKET_BUFFER, 0, (sockaddr*)&from, &fromLength);
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
