/*******************************************************************************
**                                  LibChaos                                  **
**                               zstreamsocket.h                              **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZSTREAMSOCKET_H
#define ZSTREAMSOCKET_H

#include "zsocket.h"
#include "zconnection.h"
#include "zpointer.h"

namespace LibChaos {

// For TCP sockets
// Paired with ZConnection
class ZStreamSocket : private ZSocket {
public:
    ZStreamSocket();

    bool open();
    void close();
    bool isOpen() const;

    bool bind(ZAddress port);

    void setReadBuffer(zu64 size);

    bool connect(ZAddress addr, ZConnection &conn);
    bool listen();
    ZPointer<ZConnection> accept();

    zsocktype getSocket() const {
        return ZSocket::getSocket();
    }

    ZAddress getBoundAddress() const {
        return ZSocket::getBoundAddress();
    }

    ZException getError() const {
        return ZSocket::getError();
    }

};

}

#endif // ZSTREAMSOCKET_H
