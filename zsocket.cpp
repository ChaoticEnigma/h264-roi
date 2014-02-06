#include "zsocket.h"

#include "zlog.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
//#include <stdlib.h>
//#include <fcntl.h>

#define MAXHOSTNAME 256

//#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

namespace LibChaos {

ZSocket::ZSocket() : socketHandle(0), type(udp), family(ipv4){

}

bool ZSocket::bind(socket_type typ, socket_family chn, int _port){
    type = typ;
    family = chn;
    port = _port;
    socketHandle = socket(family, type, IPPROTO_IP);
    if(socketHandle < 0){
        close();
        ELOG("Socket opening failed.");
        return false;
    }

    struct sockaddr_in socketInfo;
    memset(&socketInfo, '\0', sizeof(socketInfo));

    socketInfo.sin_family = family;
    socketInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    socketInfo.sin_port = htons(port);

    if(::bind(socketHandle, (struct sockaddr *)&socketInfo, sizeof(struct sockaddr_in)) < 0){
        close();
        ELOG("Bind failed.");
        return false;
    }

    return true;
}

bool ZSocket::listen(){
    ::listen(socketHandle, 1);

    int socketConnection = accept(socketHandle, NULL, NULL);
    if(socketConnection < 0){
       close();
       ELOG("Accept failed.");
       return false;
    }

    return true;
}

ZString ZSocket::receive(){

    return ZString();
}

void ZSocket::close(){
    ::close(socketHandle);
}

}
