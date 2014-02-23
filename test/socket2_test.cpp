#include "test.h"

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define SPLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define SPLATFORM PLATFORM_MAC
#else
#define SPLATFORM PLATFORM_UNIX
#endif

#if SPLATFORM == PLATFORM_WINDOWS

    #include <winsock2.h>

#elif SPLATFORM == PLATFORM_MAC || SPLATFORM == PLATFORM_UNIX

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>

#endif

#include <unistd.h>
#include <assert.h>

namespace net
{
    // platform independent wait for n seconds

#if SPLATFORM == PLATFORM_WINDOWS

    void wait( float seconds )
    {
        Sleep( (int) ( seconds * 1000.0f ) );
    }

#else

    #include <unistd.h>
    void wait( float seconds ) { usleep( (int) ( seconds * 1000000.0f ) ); }

#endif

    // internet address

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

    // sockets

    inline bool InitializeSockets()
    {
        #if SPLATFORM == PLATFORM_WINDOWS
        WSADATA WsaData;
        int err = WSAStartup( MAKEWORD(2,2), &WsaData );
        return err == 0;
        #else
        return true;
        #endif
    }

    inline void ShutdownSockets()
    {
        #if SPLATFORM == PLATFORM_WINDOWS
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

            #if SPLATFORM == PLATFORM_MAC || SPLATFORM == PLATFORM_UNIX

                int nonBlocking = 1;
                if ( fcntl( socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
                {
                    printf( "failed to set non-blocking socket\n" );
                    Close();
                    return false;
                }

            #elif SPLATFORM == PLATFORM_WINDOWS

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
                #if SPLATFORM == PLATFORM_MAC || SPLATFORM == PLATFORM_UNIX
                close( socket );
                #elif SPLATFORM == PLATFORM_WINDOWS
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

            #if SPLATFORM == PLATFORM_WINDOWS
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace net;

int socket2_test(){
    // initialize socket layer

    if ( !InitializeSockets() )
    {
        printf( "failed to initialize sockets\n" );
        return 1;
    }

    // create socket

    int port = 30000;

    printf( "creating socket on port %d\n", port );

    Socket socket;

    if ( !socket.Open( port ) )
    {
        printf( "failed to create socket!\n" );
        return 1;
    }

    // send and receive packets to ourself until the user ctrl-breaks...

    while ( true )
    {
        const char data[] = "hello world!";

        socket.Send( Address(127,0,0,1,port), data, sizeof(data) );

        while ( true )
        {
            Address sender;
            unsigned char buffer[256];
            int bytes_read = socket.Receive( sender, buffer, sizeof( buffer ) );
            if ( !bytes_read )
                break;
            printf( "received packet from %d.%d.%d.%d:%d (%d bytes)\n",
                sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(),
                sender.GetPort(), bytes_read );
        }

        usleep(250000);
    }

    // shutdown socket layer

    ShutdownSockets();

    return 0;
}
