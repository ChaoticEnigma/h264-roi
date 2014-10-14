#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zstring.h"
#include "zbinary.h"
#include "zerror.h"
#include "zaddress.h"

#define ZSOCKET_UDP_BUFFER  1024 * 64
#define ZSOCKET_UDP_MAX     1024 * 64 - 9

namespace LibChaos {

typedef int zsocktype;

class ZSocket {
public:
    enum socket_type {
        stream = SOCK_STREAM,
        datagram = SOCK_DGRAM,
        raw = SOCK_RAW
    };
    struct SocketOptions {
        enum socketoption {
            reuseaddr = 1,
            recvtimeout = 2,
        };
    };

public:
    ZSocket(socket_type type);
    ~ZSocket();

    bool open(ZAddress port);
    void close();
    bool isOpen() const;

    // UDP
    bool send(ZAddress destination, const ZBinary &data);
    zu32 receive(ZAddress &sender, ZBinary &str);

    // TCP
    bool connect(ZAddress addr, int &connfd, ZAddress &connaddr);
    bool listen();
    bool accept(int &connfd, ZAddress &connaddr);
    zu64 read(ZBinary &data);
    bool write(const ZBinary &data);

    bool setSocketOption(SocketOptions::socketoption option, int value);

    // Default is blocking
    bool setBlocking(bool block = true);
    // Default is non-rebind
    void allowRebind(bool rebind = false);

    void setBufferSize(zu32 size);

    zsocktype getSocket() const {
        return _socket;
    }

    ZAddress getBoundAddress() const {
        return _bound;
    }

    ZError getError() const {
        return error;
    }

protected:
    ZSocket(socket_type type, int fd);

private:
    static bool InitializeSockets();
    static void ShutdownSockets();
    bool getSocket(int &fd, ZAddress addr);

private:
    static zu32 socket_count;

    zsocktype _socket;
    socket_type _type;
    unsigned char *buffer;
    zu32 buffersize;
    bool reuseaddr;
    ZAddress _bound;
    ZError error;
};

}

#endif // ZSOCKET_H
