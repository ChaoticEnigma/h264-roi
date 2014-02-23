#include "zsocket.h"
#include "zlog.h"

#define ZSOCKET_BUFFER 1024 * 64

#include <cstring>

namespace LibChaos {

ZAddress::ZAddress() : addr(0), _port(0){}
ZAddress::ZAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short prt) : addr((a << 24) | (b << 16) | (c << 8) | d), _port(prt){}
ZAddress::ZAddress(unsigned int add, unsigned short prt) : addr(add), _port(prt){}

unsigned int ZAddress::address() const {
    return addr;
}
unsigned char ZAddress::a() const {
    return ( unsigned char ) ( addr >> 24 );
}
unsigned char ZAddress::b() const {
    return ( unsigned char ) ( addr >> 16 );
}
unsigned char ZAddress::c() const {
    return ( unsigned char ) ( addr >> 8 );
}
unsigned char ZAddress::d() const {
    return ( unsigned char ) ( addr );
}
unsigned short ZAddress::port() const {
    return _port;
}
bool ZAddress::operator ==(const ZAddress & other) const {
    return addr == other.addr && _port == other._port;
}
bool ZAddress::operator !=(const ZAddress & other) const {
    return !(*this == other);
}

// /////////////////////////////////////////////

ZSocket::ZSocket() : socket(0), buffer(NULL){
    //memset(buffer, 0, ZSOCKET_BUFFER);
}
ZSocket::~ZSocket(){
    close();
    delete buffer;
}

bool ZSocket::open(zu16 port){
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

void ZSocket::close(){
    if(socket != 0){
#if PLATFORM == LINUX
        ::close( socket );
#elif PLATFORM == WINDOWS
        closesocket( socket );
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
    address.sin_addr.s_addr = htonl( destination.address() );
    address.sin_port = htons( (unsigned short) destination.port() );
#if PLATFORM == LINUX
    long sent_bytes = sendto(socket, data.raw(), data.size(), 0, (sockaddr*)&address, sizeof(sockaddr_in));
#elif PLATFORM == WINDOWS
    long sent_bytes = sendto(socket, (const char *)data.raw(), data.size(), 0, (sockaddr*)&address, sizeof(sockaddr_in));
#endif
    return (unsigned long)sent_bytes == data.size();
}

zu32 ZSocket::receive(ZAddress &sender, ZBinary &str){
    if(socket == 0)
        return false;
    //zu64 size = 256;
    if(buffer == NULL)
        buffer = new unsigned char[ZSOCKET_BUFFER];
    //unsigned char data[size];
    sockaddr_in from;
    socklen_t fromLength = sizeof(from);
#if PLATFORM == LINUX
    long received_bytes = recvfrom(socket, buffer, ZSOCKET_BUFFER, 0, (sockaddr*)&from, &fromLength);
#elif PLATFORM == WINDOWS
    long received_bytes = recvfrom(socket, (char *)buffer, ZSOCKET_BUFFER, 0, (sockaddr*)&from, &fromLength);
#endif
    if(received_bytes <= 0)
        return 0;
    unsigned int address = ntohl(from.sin_addr.s_addr);
    unsigned int port = ntohs(from.sin_port);
    sender = ZAddress(address, port);
    zu64 len = 0;
    while(len < ZSOCKET_BUFFER){
        if(buffer[len] == 0)
            break;
        ++len;
    }
    str = ZBinary(buffer, len);
    return (zu32)received_bytes;
}

void ZSocket::listen(receiveCallback receivedFunc){
    while(true){
        ZAddress sender;
        ZBinary data;
        if(!receive(sender, data))
            continue;
        receivedFunc(sender, data);
    }
}
//void ZSocket::listen(receiveCallback receivedFunc, zu64 limit){
//    ZAddress sender;
//    ZString data;
//    if(limit == (zu64)(-1)){
//        while(true){
//            ZAddress sender;
//            unsigned char buffer[256];
//            int bytes_read = receiveraw(sender, buffer, sizeof(buffer));
//            if(!bytes_read)
//                break;
//            receivedFunc(sender, ZString((char*)buffer));
//        }
//    } else {
//        zu64 i = 0;
//        while(i < limit){
//            ZAddress sender;
//            unsigned char buffer[256];
//            int bytes_read = receiveraw(sender, buffer, sizeof(buffer));
//            if(!bytes_read)
//                break;
//            receivedFunc(sender, ZString((char*)buffer));
//        }
//    }
//}

//void ZSocket::receiveOne(receiveCallback receivedFunc){
//    while(true){
//        ZAddress sender;
//        unsigned char buffer[256];
//        int bytes_read = receiveraw(sender, buffer, sizeof(buffer));
//        if(!bytes_read)
//            break;
//        receivedFunc(sender, ZString((char*)buffer));
//    }
//}

}
