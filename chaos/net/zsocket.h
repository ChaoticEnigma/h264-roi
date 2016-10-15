/*******************************************************************************
**                                  LibChaos                                  **
**                                  zsocket.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zstring.h"
#include "zbinary.h"
#include "zexception.h"
#include "zaddress.h"

#define ZSOCKET_UDP_BUFFER  1024 * 64
#define ZSOCKET_UDP_MAX     1024 * 64 - 9

namespace LibChaos {

#if PLATFORM == WINDOWS
    typedef unsigned long long int zsocktype;
#else
    typedef int zsocktype;
#endif

//! Network socket abstraction.
class ZSocket {
public:
    enum socket_type {
        STREAM   = SOCK_STREAM,
        DATAGRAM = SOCK_DGRAM,
        RAW      = SOCK_RAW
    };

    struct SocketOptions {
        enum socketoption {
            OPT_REUSEADDR   = 1,
            OPT_RECVTIMEOUT,
        };
    };

    struct SockAddr {
        ZAddress addr;
        int type;
        int proto;
    };

public:
    ZSocket(socket_type type);
    ~ZSocket();
    ZSocket(const ZSocket &socket) = delete;

    //! Open the socket.
    bool open(int family, int type, int proto);
    //! Close the socket.
    void close();
    //! Check if socket is open.
    bool isOpen() const;

    //! Open and bind the socket to an address.
    bool bind(ZAddress port);

    // UDP
    /*! Send a datagram to \a destination with \a data.
     * \return True on success.
     */
    bool send(ZAddress destination, const ZBinary &data);
    /*! Receive a datagram, put the source address in \a sender, and datagram's data in \a data.
     * \return Size of \a data.
     */
    zu64 receive(ZAddress &sender, ZBinary &data);

    // TCP
    /*! Establish a stream connection with \a addr.
     * \param[in]  addr     Remote address.
     * \param[out] connfd   Connection socket.
     * \param[out] connaddr Connection address.
     */
    bool connect(ZAddress addr, zsocktype &connfd, ZAddress &connaddr);
    bool listen();
    bool accept(zsocktype &connfd, ZAddress &connaddr);
    zu64 read(ZBinary &data);
    bool write(const ZBinary &data);

    bool setSocketOption(SocketOptions::socketoption option, int value);

    /*! Set whether socket should be blocking or non-blocking.
     *  Default is blocking.
     */
    bool setBlocking(bool block = true);
    /*! Set whether socket should allow rebinding.
     *  This option can only take effect before binding.
     *  Default is no-rebind.
     */
    void allowRebind(bool rebind = false);

    void setBufferSize(zu32 size);

    static ZList<SockAddr> lookupAddr(ZAddress addr, int type);

    zsocktype getSocket() const {
        return _socket;
    }

    ZAddress getBoundAddress() const {
        return _bound;
    }

    ZException getError() const {
        return error;
    }

protected:
    //! Construct ZSocket from already opened socket.
    ZSocket(socket_type type, zsocktype fd);

private:
    //! Total socket count.
    static zu32 socket_count;

    //! Socket file descriptor.
    zsocktype _socket;
    //! Socket type.
    socket_type _type;
    //! Socket address family.
    ZAddress::address_family _family;
    //! Socket data buffer.
    unsigned char *buffer;
    //! Socket buffer size.
    zu32 buffersize;

    bool reuseaddr;
    ZAddress _bound;
    ZException error;
};

} // namespace LibChaos

#endif // ZSOCKET_H
