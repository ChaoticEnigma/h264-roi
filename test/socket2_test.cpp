#include "test.h"
#include "zsocket.h"
#include "zerror.h"
#include "zfile.h"

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

int tcpserver_test(){
    LOG("=== TCP Socket Server Test...");
    ZError::registerInterruptHandler(stopHandler2);
    ZError::registerSignalHandler(ZError::terminate, stopHandler2);

    ZSocket sock(ZSocket::stream);
    ZAddress bind(8080);
    LOG(bind.debugStr());
    if(!sock.open(bind)){
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
        LOG("read (" << data.size() << "): \"" << data.printable().asChar() << "\"");

        ZString str = "hello back there!";
        ZBinary snddata((const unsigned char *)str.cc(), str.size());
        client.write(snddata);
        LOG("write (" << snddata.size() << "): \"" << snddata.printable().asChar() << "\"");
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

int tcpserver_test2(){
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

        ZBinary bin;
        const long bufsize = 1024;
        unsigned char *buffer[bufsize];
        long len = 0;
        zu64 currpos = 0;
        AsArZ headers;
        zu64 currentline = 0;

        ArZ warnings;

        enum parsestate {
            beginning = 0,
            requestline = 1,

            stopparsing = 30,
        };

        parsestate state = beginning;
        zu16 breakcounter = 0;

        do {
            len = recv(new_fd, buffer, bufsize, 0);
            if(len < 0){
                ELOG("recv: " << ZError::getSystemError());
                state = stopparsing;
                break;
            } else  if(len == 0){
                state = stopparsing;
                break;
            } else if(len > 0){
                ZBinary newdata(buffer, (zu64)len);
                LOG("Got " << newdata.size());

                bin.concat(newdata);

                // Parse request header
                for(zu64 i = currpos; i < bin.size(); ++i){
                    switch(bin[i]){
                    case '\r':
                        if(state == beginning){
                            if(breakcounter == 0){
                                ++breakcounter;
                            } else {
                                warnings.push(ZString("Bad CR in request line at ") + i + " " + breakcounter);
                            }
                        }
                        break;

                    case '\n':
                        if(state == beginning){
                            if(breakcounter == 1){
                                ArZ requestparts = ZString(bin.getSub(0, i-2).asChar()).explode(' ');
                                if(requestparts.size() == 3){
                                    if(requestparts[2] != "HTTP/1.1" && requestparts[2] != "HTTP/1.0"){
                                        warnings.push(ZString("Bad HTTP-Version"));
                                        state = stopparsing;
                                        break;
                                    }
                                    breakcounter = 0;
                                    state = requestline;

                                } else {
                                    warnings.push(ZString("Bad Request-Line format"));
                                    state = stopparsing;
                                    break;
                                }
                            } else {
                                warnings.push(ZString("Bad LF in request line at ") + i + " " + breakcounter);
                            }
                        }
                        break;

                    default:
                        if(state == beginning){
                            if(breakcounter != 0){
                                warnings.push(ZString("Character inside CRLF in request line at ") + i + " " + breakcounter);
                            }
                        }
                        break;
                    }
                }
                if(state == stopparsing){
                    break;
                }

                continue;

                if(currpos == 0){ // first pass

                    for(zu64 i = currpos; i < bin.size(); ++i){
                        switch(bin[i]){
                        case ' ':
                        case '\n':
                        case '\r':

                        default:
                            break;
                        }
                    }

                    zu64 breakpos = bin.findFirst({0x0d, 0x0a, 0x0d, 0x0a});
                    LOG(breakpos);

                    if(breakpos != ZBinary::none){
                        ZBinary head = bin.getSub(0, breakpos);
                        head.nullTerm();
                        ZString header = head.asChar();
                        ArZ headers = header.strExplode("\r\n");
                        for(zu64 i = 0; i < headers.size(); ++i){
                            LOG(headers[i]);
                        }
                        break;
                    }
                } else { // continue parsing

                }
            }
        } while(len > 0);

        if(state == stopparsing){
            close(new_fd);
            break;
        }

        ZFile::writeBinary("tcpserver.bin", bin);
        LOG("Done");
        //bin.nullTerm();
        //LOG(bin.size() << " !! " << bin.asChar());
        //LOG(bin.findFirst({'H','T','T','P'}));

        //if(!fork()){ // this is the child process
            //close(sockfd); // child doesn't need the listener
            if(send(new_fd, "Hello, world!", 13, 0) == -1)
                ELOG("send");
            close(new_fd);
            //exit(0);
        //}
        //close(new_fd);  // parent doesn't need this
    }

    return 0;
}
