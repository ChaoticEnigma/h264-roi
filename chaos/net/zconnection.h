#ifndef ZCONNECTION_H
#define ZCONNECTION_H

#include "ztypes.h"
#include "zaddress.h"
#include "zbinary.h"
#include "zstring.h"

#define ZSOCKET_TCP_BUFFER  1024

namespace LibChaos {

class ZConnection {
public:
    ZConnection();
    ZConnection(int fd, ZAddress addr);

    ~ZConnection();

    void close();
    bool isOpen() const;

    zu64 read(ZBinary &str);
    bool write(const ZBinary &data);

    ZAddress other();

private:
    int _socket;
    ZAddress _addr;

    unsigned char *buffer;
};

}

#endif // ZCONNECTION_H
