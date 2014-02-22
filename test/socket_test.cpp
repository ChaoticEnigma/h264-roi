#include "test.h"
#include "zsocket.h"
#include "zthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

#if PLATFORM == WINDOWS
    #include <winsock2.h>
#elif PLATFORM == LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

namespace TCP {

//void t1(){
//    struct sockaddr_in socketInfo;
//    char sysHost[MAXHOSTNAME+1];  // Hostname of this computer we are running on
//    struct hostent *hPtr;
//    int socketHandle;
//    int portNumber = 8080;

//    signal(SIGCHLD, handleSig);

//    bzero(&socketInfo, sizeof(sockaddr_in));  // Clear structure memory

//    // Get system information

//    gethostname(sysHost, MAXHOSTNAME);  // Get the name of this computer we are running on
//    if((hPtr = gethostbyname(sysHost)) == NULL)
//    {
//      cerr << "System hostname misconfigured." << endl;
//      exit(EXIT_FAILURE);
//    }

//    // create socket

//    if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0){
//        close(socketHandle);
//        exit(EXIT_FAILURE);
//    }

//    // Load system information into socket data structures

//    socketInfo.sin_family = AF_INET;
//    socketInfo.sin_addr.s_addr = htonl(INADDR_ANY); // Use any address available to the system
//    socketInfo.sin_port = htons(portNumber);      // Set port number

//    // Bind the socket to a local socket address

//    if( bind(socketHandle, (struct sockaddr *) &socketInfo, sizeof(struct sockaddr_in)) < 0)
//    {
//      close(socketHandle);
//      perror("bind");
//      exit(EXIT_FAILURE);
//    }

//    listen(socketHandle, 1);

//    int socketConnection;
//    for(;;)  // infinite loop to handle remote connections. This should be limited.
//    {
//      if( (socketConnection = accept(socketHandle, NULL, NULL)) < 0)
//      {
//         close(socketHandle);
//         if(errno == EINTR) continue;
//         perror("accept");
//         exit(EXIT_FAILURE);
//      }
//      switch(fork())
//      {
//         case -1:
//            perror("fork");
//            close(socketHandle);
//            close(socketConnection);
//            exit(EXIT_FAILURE);
//         case 0:   // Child process - do stuff
//            close(socketHandle);
//            // Do your server stuff like read/write messages to the socket here!
//            exit(0);
//         default:    // Parent process, look for another connection
//            close(socketConnection);
//            continue;
//      }
//    }
//}



void t2(){
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8080);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while(1){
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        LOG("ACCEPT");


        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff));

        close(connfd);
        sleep(1);
     }
}



//Size of each chunk of data received, try changing this
#define CHUNK_SIZE 512

//Receiving function
int recv_timeout(int s, float);

void tcl1(){
    int socket_desc;
    struct sockaddr_in server;
    char *message;
    //char server_reply[2000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if(socket_desc == -1){
        LOG("Could not create socket");
        return;
    }

    //ip address of www.msn.com (get by doing a ping www.msn.com at terminal)
    server.sin_addr.s_addr = inet_addr("131.253.13.140");
    server.sin_family = AF_INET;
    server.sin_port = htons( 80 );

    //Connect to remote server
    if(connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0){
        LOG("connect error");
        return;
    }

    LOG("Connected");

    //Send some data
    message = (char *)"GET /?st=1 HTTP/1.1\r\nHost: www.msn.com\r\n\r\n";
    if(send(socket_desc, message, strlen(message), 0) < 0){
        LOG("Send failed");
        return;
    }

    puts("Data Send\n");
    //LOG("Data Send" << ZLog::newln); // Causes socket data to be read incorrectly

    //Now receive full data
    int total_recv = recv_timeout(socket_desc, 4);

    printf("\n\nDone. Received a total of %d bytes\n\n" , total_recv);
}

/*
    Receive data in multiple chunks by checking a non-blocking socket
    Timeout in seconds
*/
int recv_timeout(int s , float timeout){
    int size_recv , total_size= 0;
    struct timeval begin , now;
    char chunk[CHUNK_SIZE];
    double timediff;

    //make socket non blocking
    fcntl(s, F_SETFL, O_NONBLOCK);

    //beginning time
    gettimeofday(&begin , NULL);

    while(true){
        gettimeofday(&now , NULL);

        //time elapsed in seconds
        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);

        //if you got some data, then break after timeout
        if(total_size > 0 && timediff > timeout){
            break;
        }

        //if you got no data at all, wait a little longer, twice the timeout
        else if(timediff > timeout * 2){
            break;
        }

        memset(chunk ,0 , CHUNK_SIZE);
        size_recv = recv(s, chunk, CHUNK_SIZE, 0);
        if(size_recv < 0){
            //if nothing was received then we want to wait a little before trying again, 0.1 seconds
            usleep(100000);
        } else {
            total_size += size_recv;
            printf("%s" , chunk);
            //reset beginning time
            gettimeofday(&begin , NULL);
        }
    }

    return total_size;
}

void empty(){
    (void)0;
}

int t3(){
    int listenfd, connfd, n;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    //pid_t childpid;
    char mesg[1000];

    listenfd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8080);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd,1024);

    LOG("LISTENING");

    while(true){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        LOG("ACCPET");
        //empty();

        //if((childpid = fork()) == 0){
        //    close(listenfd);
            ZString req;
            while((n = recvfrom(connfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &clilen)) > 0){
//                n = recvfrom(connfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &clilen);
//                if(n <= 0){
//                    break;
//                }
                mesg[n] = 0;
                req << mesg;
//                printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
//                printf("Received the following:\n");
//                printf("%s", mesg);
//                printf("-------------------------------------------------------\n");
            }
            LOG("++++++++++++++++++++RECIEVED++++++++++++++++++++");
            RLOG(req);
            LOG("------------------------------------------------");
            sendto(connfd, mesg, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
            LOG("SENT");
        //}
        close(connfd);
    }
    return 0;
}

}

namespace UDP {

int c1(){
    int handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    if ( handle <= 0 ){
        printf( "failed to create socket\n" );
        return false;
    }

//    sockaddr_in address;
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons( (unsigned short) 8998 );

//    if ( bind( handle, (const sockaddr*) &address, sizeof(sockaddr_in) ) < 0 ){
//        printf( "failed to bind socket\n" );
//        return false;
//    }

    unsigned int a = 127;
    unsigned int b = 0;
    unsigned int c = 0;
    unsigned int d = 1;
    unsigned short port = 8998;

    unsigned int destination_address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
    unsigned short destination_port = port;

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl( destination_address );
    address.sin_port = htons( destination_port );

    const char *packet_data = "hello over there!";
    unsigned int packet_size = 17;

    unsigned sent_bytes = sendto( handle, (const char*)packet_data, packet_size, 0, (sockaddr*)&address, sizeof(sockaddr_in) );

    if ( sent_bytes != packet_size ){
        printf( "failed to send packet: return value = %d\n", sent_bytes );
        return false;
    }

    close( handle );
    return 0;
}

int s1(){
    int handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    if ( handle <= 0 ){
        printf( "failed to create socket\n" );
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( (unsigned short) 8998 );

    if ( bind( handle, (const sockaddr*) &address, sizeof(sockaddr_in) ) < 0 ){
        printf( "failed to bind socket\n" );
        return false;
    }

    int nonBlocking = 1;
    if ( fcntl( handle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 ){
        printf( "failed to set non-blocking socket\n" );
        return false;
    }

    LOG("Reading...");
    while ( true ){
        unsigned char packet_data[256];
        unsigned int maximum_packet_size = sizeof( packet_data );

        sockaddr_in from;
        socklen_t fromLength = sizeof( from );

        int received_bytes = recvfrom( handle, (char*)packet_data, maximum_packet_size, 0, (sockaddr*)&from, &fromLength );

        if ( received_bytes <= 0 )
            break;

        unsigned int from_address = ntohl( from.sin_addr.s_addr );
        unsigned int from_port = ntohs( from.sin_port );

        LOG(from_address << " " << from_port << " : " << (char *)packet_data);

        // process received packet
    }

    close( handle );
    return 0;
}

}

void *clientThread(void *zarg){
    LOG("Waiting to send...");
    sleep(1);
    LOG("Sending...");
//    UDP::c1();

    ZThreadArg *arg = (ZThreadArg*)zarg;
    while(true){
        ((ZSocket*)arg->arg)->send(ZAddress(127,0,0,1,8998), "hello world out there!");
        usleep(500000);
    }
    return NULL;
}

void receivedGram(ZAddress addr, ZString data){
    LOG(addr.a() << "." << addr.b() << "." << addr.c() << "." << addr.d() << ":" << addr.port() << " (" << data.size() << "): " << data);
}

int socket_test(){
    LOG("=== Socket Test...");
    //TCP::t2();
    //TCP::tcl1();
    //TCP::t3();

    ZSocket sock;

    if(!sock.open(8998)){
        ELOG("Socket Server Fail");
    }

    ZThread clientthr;
    clientthr.run(clientThread, (void *)&sock);

    LOG("Listening...");
    sock.listen(receivedGram);
//    ZAddress addr;
//    ZString data;
//    if(sock.receive(addr, data) > 0)
//        LOG(addr.a() << "." << addr.b() << "." << addr.c() << "." << addr.d() << ":" << addr.port() << " (" << data.size() << "): " << data);
    sock.close();

//    ZSocket sock;
//    if(sock.Open(20000)){
//        sock.Send(ZAddress(127, 0, 0, 1, 8998), "Hello there!", 13);
//        LOG("Sent.");
//    } else {
//        ELOG("Socket Server Fail");
//    }

//    LOG("Listening...");
//    //sock.listen(receivedGram);
//    while(true){
//        ZAddress addr;
//        unsigned char data[256];
//        unsigned int len = sock.Receive(addr, data, 256);
//        if(!len)
//            break;
//        LOG(addr.GetA() << "." << addr.GetB() << "." << addr.GetC() << "." << addr.GetD() << ":" << addr.GetPort() << " (" << (zu64)len << "): " << (char*)data);
//    }
//    sock.Close();

    return 0;
}

namespace UDP2 {
class Address {
public:
    Address() : addr(0), _port(0){}
    Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short prt) : addr((a << 24) | (b << 16) | (c << 8) | d), _port(prt){}
    Address(unsigned int add, unsigned short prt) : addr(add), _port(prt){}

    unsigned int address() const {
        return addr;
    }
    unsigned char a() const {
        return ( unsigned char ) ( addr >> 24 );
    }
    unsigned char b() const {
        return ( unsigned char ) ( addr >> 16 );
    }
    unsigned char c() const {
        return ( unsigned char ) ( addr >> 8 );
    }
    unsigned char d() const {
        return ( unsigned char ) ( addr );
    }
    unsigned short port() const {
        return _port;
    }
    bool operator ==(const Address & other) const {
        return addr == other.addr && _port == other._port;
    }
    bool operator !=(const Address & other) const {
        return !(*this == other);
    }
private:
    unsigned int addr;
    unsigned short _port;
};

// sockets

inline bool InitializeSockets(){
    #if PLATFORM == WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2), &WsaData ) != NO_ERROR;
    #else
    return true;
    #endif
}

inline void ShutdownSockets(){
    #if PLATFORM == WINDOWS
    WSACleanup();
    #endif
}

class Socket {
public:
    Socket() : socket(0){}
    ~Socket(){
        close();
    }

    bool open(zu16 port){
        if(isOpen())
            return false;
        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(socket <= 0){
            ELOG("failed to create socket");
            socket = 0;
            return false;
        }

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons((unsigned short) port);

        if(bind(socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0){
            ELOG("failed to bind socket");
            close();
            return false;
        }

        #if PLATFORM == LINUX
            int nonBlocking = 1;
            if(fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1){
                ELOG("failed to set non-blocking socket");
                close();
                return false;
            }
        #elif PLATFORM == WINDOWS
            DWORD nonBlocking = 1;
            if(ioctlsocket(socket, FIONBIO, &nonBlocking) != 0){
                ELOG("failed to set non-blocking socket");
                close();
                return false;
            }
        #endif
        return true;
    }

    void close(){
        if(socket != 0){
#if PLATFORM == LINUX
            ::close( socket );
#elif PLATFORM == WINDOWS
            closesocket( socket );
#endif
            socket = 0;
        }
    }

    bool isOpen() const {
        return socket != 0;
    }

    bool send(const Address &destination, const ZString &data){
        if(socket == 0)
            return false;
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl( destination.address() );
        address.sin_port = htons( (unsigned short) destination.port() );

        long sent_bytes = sendto( socket, data.cc(), data.size(), 0, (sockaddr*)&address, sizeof(sockaddr_in) );
        return (unsigned long)sent_bytes == data.size();
    }

    int receive(Address &sender, ZString &str){
        //assert(data);
        if(socket == 0)
            return false;
#if PLATFORM == WINDOWS
        typedef int socklen_t;
#endif
        zu64 size = 256;
        unsigned char data[size];
        sockaddr_in from;
        socklen_t fromLength = sizeof( from );
        int received_bytes = recvfrom( socket, (char*)data, size, 0, (sockaddr*)&from, &fromLength );
        if(received_bytes <= 0)
            return 0;
        unsigned int address = ntohl( from.sin_addr.s_addr );
        unsigned int port = ntohs( from.sin_port );
        sender = Address(address, port);
        str = ZString((char *)data);
        return received_bytes;
    }

    typedef void (*receiveCallback)(Address, ZString);

    void listen(receiveCallback receivedFunc){
        while(true){
            while(true){
                Address sender;
                ZString str;
                int bytes_read = receive(sender, str);
                if(!bytes_read)
                    break;
                receivedFunc(sender, str);
            }
        }
    }

private:
    int socket;
};
}

void *sendThread(void *zarg){
    ZThreadArg *arg = (ZThreadArg*)zarg;
    while(true){
        ((UDP2::Socket*)arg->arg)->send(UDP2::Address(127,0,0,1,8998), "hello world out there!");
        usleep(500000);
    }
    return NULL;
}

void receivedPacket(UDP2::Address sender, ZString data){
    LOG("packet " << sender.a() << "." << sender.b() << "." << sender.c() << "." << sender.d() << ":" << sender.port() << " (" << data.size() << "): " << data);
}

int socket_block2(){
    LOG("=== Socket Test 2...");
    InitializeSockets();
    int port = 8998;
    UDP2::Socket socket;
    if(!socket.open(port)){
        ELOG("failed to create socket!");
        return 1;
    }

    ZThread thr;
    thr.run(sendThread, (void*)&socket);

    socket.listen(receivedPacket);

    ShutdownSockets();
    return 0;
}
