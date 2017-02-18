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

/*! TCP interface for ZSocket.
 *  \ingroup Network
 */
class ZStreamSocket : private ZSocket {
public:
    ZStreamSocket();

    // Functions imported from ZSocket
    using ZSocket::close;
    using ZSocket::isOpen;

    using ZSocket::read;
    using ZSocket::write;

    using ZSocket::setAllowRebind;
    using ZSocket::setBlocking;
    using ZSocket::getBoundAddress;
    using ZSocket::getSocket;

    //! Open a stream connection.
    bool connect(ZAddress addr, ZConnection &conn);

    //! Open, bind and listen on a stream socket.
    bool listen(ZAddress bindaddr);

    //! Accept a new connection on the socket.
    socketerror accept(ZPointer<ZConnection> &conn);

    void setReadBuffer(zu64 size);
};

} // namespace LibChaos

#endif // ZSTREAMSOCKET_H
