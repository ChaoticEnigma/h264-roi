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

    void close();
    bool isOpen() const;

    //! Open a stream connection.
    bool connect(ZAddress addr, ZConnection &conn);

    //! Open, bind and listen on a stream socket.
    bool listen(ZAddress bindaddr);
    //! Accept a new connection on the socket.
    ZPointer<ZConnection> accept();

    void setReadBuffer(zu64 size);

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
