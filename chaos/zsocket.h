#ifndef ZSOCKET_H
#define ZSOCKET_H

#include "ztypes.h"
#include "zstring.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
//#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <assert.h>

namespace LibChaos {

class ZAddress {
public:
    ZAddress() : a(0), b (0), c(0), d(0){}
    ZAddress(ZString addr, zu16 port){
        ArZ ad = addr.explode('.');
        if(ad.size() == 4){
            a = (zu8)ad[0].tint();
            b = (zu8)ad[1].tint();
            c = (zu8)ad[2].tint();
            d = (zu8)ad[3].tint();
            _port = port;
        }
    }
    ZAddress(zu32 aaddr, zu16 aport) : addr(aaddr), _port(aport){}
    ZAddress(zu8 aa, zu8 ab, zu8 ac, zu8 ad, zu16 aport) : a(aa), b (ab), c(ac), d(ad), _port(aport){}

    zu32 address(){
        return addr;
    }
    zu16 port(){
        return _port;
    }

private:
    union {
        zu32 addr : 32;
        struct {
            zu8 a : 8;
            zu8 b : 8;
            zu8 c : 8;
            zu8 d : 8;
        };
    };
    zu16 _port : 16;
};

class ZSocket {
public:
    enum socket_type {
        tcp = SOCK_STREAM,
        udp = SOCK_DGRAM,
        raw = SOCK_RAW
    };
    enum socket_family {
        ipv4 = AF_INET,
        unix = AF_UNIX,
        ipv6 = AF_INET6
    };

    typedef void (*receiveCallback)(ZAddress, ZString);

    ZSocket();

    bool open(socket_type typ, socket_family chn, int port);
    bool isOpen();
    void close();

    bool send(ZAddress addr, const ZString &data);

    zu64 receive(ZAddress &, ZString &);
    bool listen(receiveCallback);

private:
    int socket;
    socket_type type;
    socket_family family;
    int port;
};

// /////////////////////////////////////////////////////////////////////////////////////////////

class Address
{
public:

    Address()
    {
        address = 0;
        port = 0;
    }

    Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port )
    {
        this->address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
        this->port = port;
    }

    Address( unsigned int address, unsigned short port )
    {
        this->address = address;
        this->port = port;
    }

    unsigned int GetAddress() const
    {
        return address;
    }

    unsigned char GetA() const
    {
        return ( unsigned char ) ( address >> 24 );
    }

    unsigned char GetB() const
    {
        return ( unsigned char ) ( address >> 16 );
    }

    unsigned char GetC() const
    {
        return ( unsigned char ) ( address >> 8 );
    }

    unsigned char GetD() const
    {
        return ( unsigned char ) ( address );
    }

    unsigned short GetPort() const
    {
        return port;
    }

    bool operator == ( const Address & other ) const
    {
        return address == other.address && port == other.port;
    }

    bool operator != ( const Address & other ) const
    {
        return ! ( *this == other );
    }

private:

    unsigned int address;
    unsigned short port;
};

inline bool InitializeSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2), &WsaData ) != NO_ERROR;
    #else
    return true;
    #endif
}

inline void ShutdownSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
    #endif
}

class Socket
{
public:

    Socket()
    {
        socket = 0;
    }

    ~Socket()
    {
        Close();
    }

    bool Open( unsigned short port )
    {
        assert( !IsOpen() );

        // create socket

        socket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

        if ( socket <= 0 )
        {
            printf( "failed to create socket\n" );
            socket = 0;
            return false;
        }

        // bind to port

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( (unsigned short) port );

        if ( bind( socket, (const sockaddr*) &address, sizeof(sockaddr_in) ) < 0 )
        {
            printf( "failed to bind socket\n" );
            Close();
            return false;
        }

        // set non-blocking io

        #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

            int nonBlocking = 1;
            if ( fcntl( socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
            {
                printf( "failed to set non-blocking socket\n" );
                Close();
                return false;
            }

        #elif PLATFORM == PLATFORM_WINDOWS

            DWORD nonBlocking = 1;
            if ( ioctlsocket( socket, FIONBIO, &nonBlocking ) != 0 )
            {
                printf( "failed to set non-blocking socket\n" );
                Close();
                return false;
            }

        #endif

        return true;
    }

    void Close()
    {
        if ( socket != 0 )
        {
            #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
            close( socket );
            #elif PLATFORM == PLATFORM_WINDOWS
            closesocket( socket );
            #endif
            socket = 0;
        }
    }

    bool IsOpen() const
    {
        return socket != 0;
    }

    bool Send( const Address & destination, const void * data, int size )
    {
        assert( data );
        assert( size > 0 );

        if ( socket == 0 )
            return false;

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl( destination.GetAddress() );
        address.sin_port = htons( (unsigned short) destination.GetPort() );

        int sent_bytes = sendto( socket, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in) );

        return sent_bytes == size;
    }

    int Receive( Address & sender, void * data, int size )
    {
        assert( data );
        assert( size > 0 );

        if ( socket == 0 )
            return false;

        #if PLATFORM == PLATFORM_WINDOWS
        typedef int socklen_t;
        #endif

        sockaddr_in from;
        socklen_t fromLength = sizeof( from );

        int received_bytes = recvfrom( socket, (char*)data, size, 0, (sockaddr*)&from, &fromLength );

        if ( received_bytes <= 0 )
            return 0;

        unsigned int address = ntohl( from.sin_addr.s_addr );
        unsigned int port = ntohs( from.sin_port );

        sender = Address( address, port );

        return received_bytes;
    }

private:

    int socket;
};

}

#endif // ZSOCKET_H
