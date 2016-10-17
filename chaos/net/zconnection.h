/*******************************************************************************
**                                  LibChaos                                  **
**                                zconnection.h                               **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZCONNECTION_H
#define ZCONNECTION_H

#include "ztypes.h"
#include "zsocket.h"
#include "zaddress.h"

namespace LibChaos {

/*! TCP connection abstraction of ZSocket.
 *  \ingroup Network
 */
class ZConnection : private ZSocket {
public:
    //! Construct unopened connection.
    ZConnection();
    //! Construct from opened socket connected to addr.
    ZConnection(zsocktype fd, ZAddress addr);
    //! Destructor will close the connection socket.
    ~ZConnection();

    // Functions imported from ZSocket
    using ZSocket::close;
    using ZSocket::isOpen;

    using ZSocket::read;
    using ZSocket::write;

    using ZSocket::setBlocking;
    using ZSocket::getSocket;

    //! Get the address of the peer.
    ZAddress peer();

private:
    ZAddress _peeraddr;
    unsigned char *buffer;
};

} // namespace LibChaos

#endif // ZCONNECTION_H
