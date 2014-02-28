#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zaddress.h"
#include "zbinary.h"
#include "zstring.h"
#include "zconnection.h"

#define ZSOCKET_UDP_BUFFER  1024 * 64
#define ZSOCKET_UDP_MAX     1024 * 64 - 9

namespace LibChaos {

class ZConnection;

class ZSocket {
public:
    typedef void (*receiveCallback)(ZSocket *_socket, const ZAddress &sender, const ZBinary &data);

    enum socket_type {
        tcp = SOCK_STREAM,
        udp = SOCK_DGRAM,
        raw = SOCK_RAW
    };

    ZSocket(socket_type type);
    ~ZSocket();

    bool open(ZAddress port);
    void close();
    bool isOpen() const;

    // UDP
    bool send(ZAddress destination, const ZBinary &data);
    zu32 receive(ZAddress &sender, ZBinary &str);
    void receiveFunc(receiveCallback receivedFunc);

    // TCP
    bool connect(ZAddress addr, ZConnection &conn);
    bool listen();
    bool accept(ZConnection &conn);


    bool setBlocking(bool);
    void allowRebind(bool);

    int getHandle() const {
        return _socket;
    }

    ZAddress getBound(){
        return _bound;
    }

private:
    static bool InitializeSockets();
    static void ShutdownSockets();

    static zu64 socket_count;

    int _socket;
    int _type;
    unsigned char *buffer;

    bool reuseaddr;

    ZAddress _bound;
};

}

#endif // ZSOCKET_H
