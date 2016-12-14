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
#include "zaddress.h"
#include "zexception.h"

#define ZSOCKET_UDP_BUFFER  1024 * 64
#define ZSOCKET_UDP_MAX     1024 * 64 - 9

namespace LibChaos {

#if PLATFORM == WINDOWS
    typedef unsigned long long int zsocktype;
#else
    typedef int zsocktype;
#endif

/*! Network socket abstraction.
 *  \ingroup Network
 */
class ZSocket {
public:
    enum socket_type {
        STREAM   = SOCK_STREAM,
        DATAGRAM = SOCK_DGRAM,
        RAW      = SOCK_RAW
    };

    struct SocketOptions {
        enum socketoption {
            OPT_REUSEADDR = 1,
            OPT_RECVTIMEOUT,
        };
    };

    enum socketerror {
        OK = 0,         //!< No error.
        DONE,           //!< Nothing to read/write.
        AGAIN,          //!< Socket operation would block.
        ERR_NOTOPEN,    //!< Socket is not open.
        ERR_READ,       //!< Read error.
        ERR_WRITE,      //!< Write error.
        ERR_ACCEPT,     //!< Accept error.
        ERR_BUFFER,     //!< No buffer.
        ERR_MAX,
    };

    struct SockAddr {
        ZAddress addr;
        int type;
        int proto;
    };

public:
    //! Construct new socket with type.
    ZSocket(socket_type type);
    // No copy constructor
    ZSocket(const ZSocket &socket) = delete;
    //! Destructor will not close the socket.
    ~ZSocket();

    //! Open the socket.
    bool open(int family, int type, int proto);
    //! Close the socket.
    void close();
    //! Check if socket is open.
    bool isOpen() const;

    //! Open and bind the socket to an address.
    bool bind(ZAddress port);

    //! \name TCP-Specific Members
    //! \{

    /*! Establish a stream connection with \a addr.
     *  \param[in]  addr     Remote address.
     *  \param[out] connfd   Connection socket.
     *  \param[out] connaddr Connection address.
     */
    bool connect(ZAddress addr, zsocktype &connfd, ZAddress &connaddr);

    //! Start listening on the socket.
    bool listen();

    //! Accept a TCP connection on the socket.
    socketerror accept(zsocktype &connfd, ZAddress &connaddr);

    /*! Read from the socket into \p data.
     *  Will read up to the size of \p data, so \p data must be resized first.
     *  \p data will be resized to the size actually read.
     *  \return \ref OK on success.
     *  \return \ref DONE when no data read.
     *  \return \ref AGAIN when read would block.
     *  \return \ref ERR_READ on read failure.
     *  \return \ref ERR_BUFFER when \p data is empty.
     */
    socketerror read(ZBinary &data);

    /*! Write the contents of \p data to the socket.
     *  \return \ref OK on success.
     *  \return \ref DONE when no data written.
     *  \return \ref AGAIN when write would block.
     *  \return \ref ERR_WRITE on write failure.
     *  \return \ref ERR_BUFFER when \p data is empty.
     */
    socketerror write(const ZBinary &data);

    //! \}

    //! \name UDP-Specific Members
    //! \{

    /*! Send a datagram to \a destination with \a data.
     *  \return True on success.
     */
    socketerror send(ZAddress destination, const ZBinary &data);

    /*! Receive a datagram, put the source address in \a sender, and datagram's data in \a data.
     *  \return Size of \a data.
     */
    socketerror receive(ZAddress &sender, ZBinary &data);

    //! \}

    //! Set a socket option.
    bool setSocketOption(SocketOptions::socketoption option, int value);

    /*! Set whether socket should allow rebinding.
     *  This option can only take effect before binding.
     *  Default is no-rebind.
     */
    void setAllowRebind(bool rebind = false);

    /*! Set whether socket should be blocking or non-blocking.
     *  Default is blocking.
     */
    bool setBlocking(bool block = true);

    //! Look up an address for a list of candidate socket addresses.
    static ZList<SockAddr> lookupAddr(ZAddress addr, int type);

    //! Get underlying socket handle.
    inline zsocktype getSocket() const { return _socket; }
    //! Get the bound address.
    inline ZAddress getBoundAddress() const { return _bound; }

    static ZString errorStr(socketerror err);

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
    //! Whether reused address options should be set.
    bool _reuseaddr;
    //! Socket bound address.
    ZAddress _bound;
};

} // namespace LibChaos

#endif // ZSOCKET_H
