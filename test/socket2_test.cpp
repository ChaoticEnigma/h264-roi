#include "test.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

#if PLATFORM == WINDOWS
    #include <winsock2.h>
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

class Address {
public:
    Address() : addr(0), _port(0){}
    Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short prt) : addr((a << 24) | (b << 16) | (c << 8) | d), _port(prt){}
    Address(unsigned int add, unsigned short prt) : addr(add), _port(prt){}

    unsigned int address() const {
        return addr;
    }
    unsigned char a() const {
        return ( unsigned char ) ( addr >> 24 );
    }
    unsigned char b() const {
        return ( unsigned char ) ( addr >> 16 );
    }
    unsigned char c() const {
        return ( unsigned char ) ( addr >> 8 );
    }
    unsigned char d() const {
        return ( unsigned char ) ( addr );
    }
    unsigned short port() const {
        return _port;
    }
    bool operator ==(const Address & other) const {
        return addr == other.addr && _port == other._port;
    }
    bool operator !=(const Address & other) const {
        return !(*this == other);
    }
private:
    unsigned int addr;
    unsigned short _port;
};

// sockets

inline bool InitializeSockets(){
    #if PLATFORM == WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2), &WsaData ) != NO_ERROR;
    #else
    return true;
    #endif
}

inline void ShutdownSockets(){
    #if PLATFORM == WINDOWS
    WSACleanup();
    #endif
}

class Socket {
public:
    Socket() : socket(0){}
    ~Socket(){
        close();
    }

    bool open(zu16 port){
        if(isOpen())
            return false;
        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(socket <= 0){
            ELOG("failed to create socket");
            socket = 0;
            return false;
        }

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons((unsigned short) port);

        if(bind(socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0){
            ELOG("failed to bind socket");
            close();
            return false;
        }

        #if PLATFORM == LINUX
            int nonBlocking = 1;
            if(fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1){
                ELOG("failed to set non-blocking socket");
                close();
                return false;
            }
        #elif PLATFORM == WINDOWS
            DWORD nonBlocking = 1;
            if(ioctlsocket(socket, FIONBIO, &nonBlocking) != 0){
                ELOG("failed to set non-blocking socket");
                close();
                return false;
            }
        #endif
        return true;
    }

    void close(){
        if(socket != 0){
#if PLATFORM == LINUX
            ::close( socket );
#elif PLATFORM == WINDOWS
            closesocket( socket );
#endif
            socket = 0;
        }
    }

    bool isOpen() const {
        return socket != 0;
    }

    bool send(const Address & destination, const void * data, zu64 size){
        assert(data);
        if(socket == 0)
            return false;
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl( destination.address() );
        address.sin_port = htons( (unsigned short) destination.port() );

        int sent_bytes = sendto( socket, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in) );
        return sent_bytes == size;
    }

    int receive(Address & sender, void * data, zu64 size){
        assert(data);
        if(socket == 0)
            return false;
#if PLATFORM == WINDOWS
        typedef int socklen_t;
#endif
        sockaddr_in from;
        socklen_t fromLength = sizeof( from );
        int received_bytes = recvfrom( socket, (char*)data, size, 0, (sockaddr*)&from, &fromLength );
        if(received_bytes <= 0)
            return 0;
        unsigned int address = ntohl( from.sin_addr.s_addr );
        unsigned int port = ntohs( from.sin_port );
        sender = Address(address, port);
        return received_bytes;
    }

    typedef void (*receiveCallback)(Address, ZString);

    void listen(receiveCallback receivedFunc){
        while(true){
            while(true){
                Address sender;
                unsigned char buffer[256];
                int bytes_read = receive(sender, buffer, sizeof(buffer));
                if(!bytes_read)
                    break;
                receivedFunc(sender, ZString((char*)buffer));
            }
        }
    }

private:
    int socket;
};

void *sendThread(void *zarg){
    ZThreadArg *arg = (ZThreadArg*)zarg;
    const char data[] = "hello world out there!";
    while(true){
        ((Socket*)arg->arg)->send(Address(127,0,0,1,8998), data, sizeof(data));
        usleep(500000);
    }
    return NULL;
}

void receivedPacket(Address sender, ZString data){
    LOG("packet " << sender.a() << "." << sender.b() << "." << sender.c() << "." << sender.d() << ":" << sender.port() << " (" << data.size() << "): " << data);
}

int socket_block2(){
    LOG("=== Socket Test 2...");
    InitializeSockets();
    int port = 8998;
    Socket socket;
    if(!socket.open(port)){
        ELOG("failed to create socket!");
        return 1;
    }

    ZThread thr;
    thr.run(sendThread, (void*)&socket);

    socket.listen(receivedPacket);

    ShutdownSockets();
    return 0;
}
