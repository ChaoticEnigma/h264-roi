#include "test.h"
#include "zsocket.h"

#include <string.h>

int lookup(const char *name, const char *port){
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if((status = getaddrinfo(name, port, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    RLOG("IP addresses for " << name << ":" << port << " -" << ZLog::newln);

    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if(p->ai_family == AF_INET){ // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            RLOG("  IPv4 -" << p->ai_family << "- " << ipstr << " ("<< p->ai_addrlen << ") : " << p->ai_canonname << " : " << p->ai_protocol << " " << p->ai_socktype << ZLog::newln);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
            RLOG("  IPv6 -" << p->ai_family << "- " << ipstr << " ("<< p->ai_addrlen << ") : " << p->ai_canonname << " : " << p->ai_protocol << " " << p->ai_socktype << ZLog::newln);
        }
    }

    freeaddrinfo(res); // free the linked list
    return 0;
}

int tcp_test(){
    LOG("=== TCP Socket Test...");

    lookup("127.0.0.1", "8080");
    lookup("google.com", "http");
    //lookup("google.com", "dns");
    lookup("microsoft.com", "80");

    ZSocket sock;
    //sock.openStream(ZAddress(0, 8998));
    sock.openStream(ZAddress("www.google.com"));
    sock.openStream(ZAddress("www.znnxs.com"));

    return 0;
}
int tcpserver_test(){
    LOG("=== TCP Socket Server Test...");

    return 0;
}
