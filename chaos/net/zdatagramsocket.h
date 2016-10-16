/*******************************************************************************
**                                  LibChaos                                  **
**                              zdatagramsocket.h                             **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZDATAGRAMSOCKET_H
#define ZDATAGRAMSOCKET_H

#include "zsocket.h"

namespace LibChaos {

/*! UDP interface for ZSocket.
 *  \ingroup Network
 */
class ZDatagramSocket : private ZSocket {
public:
    typedef void (*receiveCallback)(ZSocket *_socket, const ZAddress &sender, const ZBinary &data);

public:
    ZDatagramSocket();

    bool open();

    // Functions imported from ZSocket
    using ZSocket::close;
    using ZSocket::isOpen;

    using ZSocket::bind;
    using ZSocket::send;
    using ZSocket::receive;

    using ZSocket::setBlocking;
    using ZSocket::setBufferSize;
    using ZSocket::getBoundAddress;
    using ZSocket::getSocket;
    using ZSocket::getError;

    void setReadBuffer(zu64 size);

    void receiveFunc(receiveCallback receivedFunc);
};

} // namespace LibChaos

#endif // ZDATAGRAMSOCKET_H
