#include "zsocket.h"
#include "zlog.h"

#define MAXHOSTNAME 256

//#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

namespace LibChaos {

ZSocket::ZSocket() : socket(0), type(udp), family(ipv4){}

bool ZSocket::open(socket_type typ, socket_family chn, int _port){
    if(!isOpen()){
        type = typ;
        family = chn;
        port = _port;

        if(type == udp){
            socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(socket <= 0){
                ELOG("Socket opening failed.");
                socket = 0;
                return false;
            }

            sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons((unsigned short)port);

            if(::bind(socket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0){
                close();
                ELOG("Bind failed.");
                return false;
            }

#if PLATFORM == LINUX
            int nonBlocking = 1;
            if(::fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1){
                ELOG("failed to set non-blocking socket");
                close();
                return false;
            }
#elif PLATFORM == WINDOWS
            DWORD nonBlocking = 1;
            if(::ioctlsocket(socket, FIONBIO, &nonBlocking) != 0){
                ELOG("failed to set non-blocking socket");
                close();
                return false;
            }
#endif
            return true;
        }
    }
    return false;
}

bool ZSocket::isOpen(){
    return (socket != 0);
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

bool ZSocket::send(ZAddress addr, const ZString &data){
    if(socket == 0)
        return false;

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = ::htonl(addr.address());
    address.sin_port = ::htons(addr.port());

    long sent_bytes = ::sendto(socket, data.cc(), data.size(), 0, (sockaddr*)&address, sizeof(sockaddr_in));

    return ((zu64)sent_bytes == data.size());
}

zu64 ZSocket::receive(ZAddress &addr, ZString &str){
    if(socket == 0)
        return 0;

    sockaddr_in from;
    socklen_t fromLength = sizeof(from);

    char *buffer = new char[256];
    int received_bytes = recvfrom(socket, (char*)buffer, 256, 0, (sockaddr*)&from, &fromLength);

    if(received_bytes <= 0)
        return 0;

    unsigned int address = ntohl(from.sin_addr.s_addr);
    unsigned short port = ntohs(from.sin_port);

    str = buffer;
    addr = ZAddress(address, port);

    return received_bytes;
}

bool ZSocket::listen(receiveCallback callback){
    while(true){
        ZAddress addr;
        ZString str;
        if(receive(addr, str) > 0){
            callback(addr, str);
        }
    }

    return true;
}

}
