/*******************************************************************************
**                                  LibChaos                                  **
**                             zdatagramsocket.cpp                            **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zdatagramsocket.h"

namespace LibChaos {

ZDatagramSocket::ZDatagramSocket() : ZSocket(ZSocket::DATAGRAM){
    setAllowRebind(true);
}

bool ZDatagramSocket::open(){
    return ZSocket::open(ZAddress::IPV6, ZSocket::DATAGRAM, 0);
}

void ZDatagramSocket::receiveFunc(receiveCallback receivedFunc){
    while(true){
        ZAddress sender;
        ZBinary data;
        if(!receive(sender, data))
            continue;
        receivedFunc(this, sender, data);
    }
}

}
