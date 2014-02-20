#include "zsocket.h"

#include "zlog.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
//#include <stdlib.h>
#include <fcntl.h>

#define MAXHOSTNAME 256

//#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

namespace LibChaos {

ZSocket::ZSocket() : socketHandle(0), isopen(false), type(udp), family(ipv4){

}

bool ZSocket::open(socket_type typ, socket_family chn, int _port){
    isopen = false;
    type = typ;
    family = chn;
    port = _port;

    if(type == udp){
        socketHandle = socket(family, SOCK_DGRAM, IPPROTO_UDP);
        if(socketHandle <= 0){
            ELOG("Socket opening failed.");
            socketHandle = 0;
            return false;
        }

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons((unsigned short)port);

        if(bind(socketHandle, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0){
            close();
            ELOG("Bind failed.");
            return false;
        }

        int nonBlocking = 1;
        if(fcntl(socketHandle, F_SETFL, O_NONBLOCK, nonBlocking) == -1){
            close();
            ELOG("Set non-blocking failed.");
            return false;
        }

        isopen = true;
        return true;
    } else {
        return false;
    }
}

bool ZSocket::isOpen(){
    return isopen;
}

void ZSocket::close(){
    ::close(socketHandle);
    isopen = false;
}

bool ZSocket::send(ZAddress addr, const ZString &data){
    if(socketHandle == 0)
        return false;

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(addr.address());
    address.sin_port = htons(addr.port());

    zu64 sent_bytes = ::sendto(socketHandle, data.cc(), data.size(), 0, (sockaddr*)&address, sizeof(sockaddr_in));

    return (sent_bytes == data.size());
}

zu64 ZSocket::receive(ZAddress &addr, ZString &str){
    if(socketHandle == 0)
        return 0;

    sockaddr_in from;
    socklen_t fromLength = sizeof(from);

    char *buffer = new char[256];
    int received_bytes = recvfrom(socketHandle, (char*)buffer, 256, 0, (sockaddr*)&from, &fromLength);

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
