#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zstring.h"

#include "zlog.h"

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

namespace LibChaos {

class ZAddress {
public:
    ZAddress();
    ZAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short prt);
    ZAddress(unsigned int add, unsigned short prt);

    unsigned int address() const;
    unsigned char a() const;
    unsigned char b() const;
    unsigned char c() const;
    unsigned char d() const;
    unsigned short port() const;
    ZString str();

    bool operator ==(const ZAddress & other) const;
    bool operator !=(const ZAddress & other) const;
private:
    unsigned int addr;
    unsigned short _port;
};

// sockets

class ZSocket {
public:
    typedef void (*receiveCallback)(ZAddress, ZBinary);
#if PLATFORM == WINDOWS
    typedef int socklen_t;
#endif

    ZSocket();
    ~ZSocket();

    bool open(zu16 port);
    void close();
    bool isOpen() const;

    bool send(const ZAddress &destination, const ZBinary &data);

    //zu32 receiveraw(ZAddress & sender, void * data, zu64 size);
    zu32 receive(ZAddress &sender, ZBinary &str);
    void listen(receiveCallback receivedFunc);
    //void listen(receiveCallback receivedFunc, zu64 limit = -1);

    static inline bool InitializeSockets(){
#if PLATFORM == WINDOWS
        WSADATA WsaData;
        return WSAStartup(MAKEWORD(2,2), &WsaData) == 0;
#else
        return true;
#endif
    }

    static inline void ShutdownSockets(){
#if PLATFORM == WINDOWS
        WSACleanup();
#endif
    }

private:
    //void receiveOne(receiveCallback receivedFunc);

    int socket;
    unsigned char *buffer;
};

}

#endif // ZSOCKET_H
