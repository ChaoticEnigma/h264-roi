#include "zuid.h"

#include <time.h>

#include "zlog.h"

#include <windows.h>
#include <iptypes.h>
#include <iphlpapi.h>

namespace LibChaos {

ZBinary GetMACaddress(void){
    IP_ADAPTER_INFO AdapterInfo[16];
    ULONG dwBufLen = sizeof(AdapterInfo);

    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);

    ArZ mac;
    for(zu64 i = 0; i < 6; ++i)
        mac.push(ZString::ItoS(AdapterInfo[0].Address[i], 16, 2));
    LOG(ZString::compound(mac, "-"));

    ZBinary bin;
    bin.write(AdapterInfo[0].Address, 6);
    return bin;

    //    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
//    do {
//        PrintMACaddress(AdapterInfo->Address); // Print MAC address
//        pAdapterInfo = pAdapterInfo->Next; // Progress through
//        // linked list
//    } while(pAdapterInfo); // Terminate if last adapter
}

ZUID::ZUID(){
    _id.first = 0;
    _id.second = 0;

    ZBinary mac = GetMACaddress();
    LOG(mac.getPos());
    mac.rewind();
    mac.read(_id.octets + 10, 6);
//    for(zu64 i = 0; i < 6; ++i)
//        _id.octets[10+i] = mac[i];

    ArZ macaddr;
    for(zu64 i = 0; i < 6; ++i)
        macaddr.push(ZString::ItoS(mac[i], 16, 2));
    LOG(ZString::compound(macaddr, "-"));

    //clock_t clicks = clock();
    //time_t rawtime = time(NULL);
    //tm time = gmtime(rawtime);

}

ZUID::ZUID(ZString str){
    str.replace("-", "");
    if(str.size() == 32 && str.isInteger(16)){
        ZString p1 = ZString::substr(str, 0, 16);
        ZString p2 = ZString::substr(str, 16, 16);
        _id.first = p1.tozu64(16);
        _id.second = p2.tozu64(16);
    }
}

ZString ZUID::str() const {
    ZString uid;
    for(zu8 i = 0; i < 16; ++i)
        uid += ZString::ItoS(_id.octets[i], 16, 2);
//    ZString p1 = ZString::ItoS(_id.first, 16, 16);
//    ZString p2 = ZString::ItoS(_id.second, 16, 16);
//    ZString str = p1 + p2;
    uid.insert(8, "-");
    uid.insert(8 + 1 + 4, "-");
    uid.insert(8 + 1 + 4 + 1 + 4, "-");
    uid.insert(8 + 1 + 4 + 1 + 4 + 1 + 4, "-");
    return uid;
}

}
