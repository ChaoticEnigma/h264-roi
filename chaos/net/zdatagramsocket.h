/*******************************************************************************
**                                  LibChaos                                  **
**                              zdatagramsocket.h                             **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZDATAGRAMSOCKET_H
#define ZDATAGRAMSOCKET_H

#include "zsocket.h"

namespace LibChaos {

// For UDP sockets
class ZDatagramSocket : private ZSocket {
public:
    typedef void (*receiveCallback)(ZSocket *_socket, const ZAddress &sender, const ZBinary &data);

public:
    ZDatagramSocket();

    void close();
    bool isOpen() const;

    bool bind(ZAddress port);

    void setReadBuffer(zu64 size);

    bool send(ZAddress destination, const ZBinary &data);
    zu32 receive(ZAddress &sender, ZBinary &str);
    void receiveFunc(receiveCallback receivedFunc);

    ZAddress getBoundAddress() const {
        return ZSocket::getBoundAddress();
    }

    ZException getError() const {
        return ZSocket::getError();
    }

};

}

#endif // ZDATAGRAMSOCKET_H
