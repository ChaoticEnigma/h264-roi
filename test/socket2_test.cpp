#include "test.h"
#include "zsocket.h"
#include "zerror.h"

static bool run = true;

void stopHandler2(ZError::zerror_signal sig){
    run = false;
}

int tcp_test(){
    LOG("=== TCP Socket Test...");
    ZError::registerInterruptHandler(stopHandler2);
    ZError::registerSignalHandler(ZError::terminate, stopHandler2);

    ZSocket sock(ZSocket::stream);

    //ZAddress addr("::1", 8998);
    ZAddress addr("127.0.0.1", 8998);
    //ZAddress addr("192.168.1.71", 8998);
    //ZAddress addr("192.168.1.89", 8998);

    ZConnection conn;
    LOG("Connecting to " << addr.str());
    if(!sock.connect(addr, conn)){
        ELOG("Socket Connect Fail");
        return 3;
    }
    LOG("connected " << conn.other().str());

    ZString str = "hi connection!";
    ZBinary snddata((const unsigned char *)str.cc(), str.size());
    conn.write(snddata);
    LOG("write (" << snddata.size() << "): \"" << snddata << "\"");

    ZBinary data;
    conn.read(data);
    LOG("read (" << data.size() << "): \"" << data << "\"");

    return 0;
}

int tcpserver_test2(){
    LOG("=== TCP Socket Server Test...");
    ZError::registerInterruptHandler(stopHandler2);
    ZError::registerSignalHandler(ZError::terminate, stopHandler2);

    ZSocket sock(ZSocket::stream);
    if(!sock.open(ZAddress(8080))){
        ELOG("Socket Open Fail");
        return 2;
    }
    if(!sock.listen()){
        ELOG("Socket Listen Fail");
        return 3;
    }

    LOG("Listening...");

    while(run){
        ZConnection client;
        if(!sock.accept(client))
            continue;
        LOG("accept " << client.other().debugStr());

        ZBinary data;
        client.read(data);
        LOG("read (" << data.size() << "): \"" << data << "\"");

        ZString str = "hello back there!";
        ZBinary snddata((const unsigned char *)str.cc(), str.size());
        client.write(snddata);
        LOG("write (" << snddata.size() << "): \"" << snddata << "\"");
    }

    return 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "8080"  // the port users will be connecting to

#define BACKLOG 10  // how many pending connections queue will hold

void sigchld_handler(int s){
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else if(sa->sa_family == AF_INET6){
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    } else {
        throw ZError("Invalid sa_family");
    }
}

int tcpserver_test(){
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
        ELOG("getaddrinfo: " << gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            ELOG("server: socket");
            continue;
        }

        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            ELOG("setsockopt");
            exit(1);
        }

        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            ELOG("server: bind");
            continue;
        }

        break;
    }

    if(p == NULL){
        ELOG("server: failed to bind");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if(listen(sockfd, BACKLOG) == -1){
        ELOG("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        ELOG("sigaction");
        exit(1);
    }

    LOG("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if(new_fd == -1){
            ELOG("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        LOG("server: got connection from " << s);

        ZAddress addr((sockaddr *)&their_addr);
        LOG(addr.debugStr());

        if(!fork()){ // this is the child process
            close(sockfd); // child doesn't need the listener
            if(send(new_fd, "Hello, world!", 13, 0) == -1)
                ELOG("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}
