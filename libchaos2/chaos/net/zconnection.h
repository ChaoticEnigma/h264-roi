#ifndef ZCONNECTION_H
#define ZCONNECTION_H

#include "ztypes.h"
#include "zaddress.h"
#include "zbinary.h"
#include "zstring.h"
#include "zsocket.h"

namespace LibChaos {

// For TCP connections
class ZConnection : private ZSocket {
public:
    ZConnection();
    ZConnection(int fd, ZAddress addr);

    ~ZConnection();

    void close();
    bool isOpen() const;

    zu64 read(ZBinary &str);
    bool write(const ZBinary &data);

    void setBufferSize(zu32 size){
        ZSocket::setBufferSize(size);
    }

    ZAddress other();

    zsocktype getSocket() const {
        return ZSocket::getSocket();
    }

    ZError getError() const {
        return ZSocket::getError();
    }

private:
    int _socket;
    ZAddress _addr;

    unsigned char *buffer;
};

}

#endif // ZCONNECTION_H
