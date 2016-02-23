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

class ZSocket {
public:
    enum socket_type {
        STREAM = SOCK_STREAM,
        DATAGRAM = SOCK_DGRAM,
        RAW = SOCK_RAW
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

    //! Open the socket.
    bool open();
    //! Bind the socket to an address.
    bool bind(ZAddress port);
    //! Close the socket.
    void close();
    //! Get if socket is open.
    bool isOpen() const;

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

    ZException getError() const {
        return error;
    }

protected:
    ZSocket(socket_type type, zsocktype fd);

private:
    static bool InitializeSockets();
    static void ShutdownSockets();
    bool getSocket(zsocktype &fd, int type, int proto);

private:
    static zu32 socket_count;

    zsocktype _socket;
    socket_type _type;
    unsigned char *buffer;
    zu32 buffersize;
    bool reuseaddr;
    ZAddress _bound;
    ZException error;
};

}

#endif // ZSOCKET_H
