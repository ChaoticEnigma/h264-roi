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

ZSocket::ZSocket() : socketHandle(0), type(udp), channel(ipv4){

}

bool ZSocket::open(socket_type typ, socket_channel chn){
    type = typ;
    channel = chn;
    socketHandle = socket(channel, type, IPPROTO_IP);
    if(socketHandle < 0){
        close();
        ELOG("Socket opening failed.");
        return false;
    }
    return true;
}

bool ZSocket::listen(int _port){
    port = _port;

    struct sockaddr_in socketInfo;
    //bzero(&socketInfo, sizeof(sockaddr_in));
    memset(&socketInfo, '\0', sizeof(sockaddr_in));

    char sysHost[MAXHOSTNAME+1];
    gethostname(sysHost, MAXHOSTNAME);
    //struct hostNamePtr *hPtr;
    //hPtr = gethostbyname(sysHost);
    //if(hPtr == NULL){
    //   ELOG("System hostname misconfigured.");
    //   return false;
    //}

    socketInfo.sin_family = channel;
    socketInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    socketInfo.sin_port = htons(port);

    if(bind(socketHandle, (struct sockaddr *)&socketInfo, sizeof(struct sockaddr_in)) < 0){
        close();
        ELOG("Bind failed.");
        return false;
    }

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
