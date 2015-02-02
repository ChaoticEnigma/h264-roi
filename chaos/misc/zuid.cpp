#include "zuid.h"
#include "zlog.h"

#include <time.h>

#if PLATFORM == WINDOWS
    #include <windows.h>
    #include <iptypes.h>
    #include <iphlpapi.h>
#else
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <unistd.h>
    #include <netinet/in.h>
#endif

namespace LibChaos {

ZBinary getMACAddress(){
#if PLATFORM == WINDOWS
    PIP_ADAPTER_INFO adapterInfo;
    ULONG bufLen = sizeof(IP_ADAPTER_INFO);
    adapterInfo = new IP_ADAPTER_INFO[1];

    // Get number of adapters and list of adapter info
    DWORD status = GetAdaptersInfo(adapterInfo, &bufLen);
    if(status == ERROR_BUFFER_OVERFLOW){
        delete[] adapterInfo;
        adapterInfo = new IP_ADAPTER_INFO[bufLen];
        status = GetAdaptersInfo(adapterInfo, &bufLen);
    }
    // Get MAC of first adapter
    if(status == NO_ERROR){
        // Loop through linked list of adapters
        PIP_ADAPTER_INFO adapterInfoList = adapterInfo;
        do {
            ArZ mac;
            for(zu64 i = 0; i < 6; ++i)
                mac.push(ZString::ItoS(adapterInfoList->Address[i], 16, 2));
            LOG(ZString::compound(mac, ":") << " " << adapterInfoList->Description);
            adapterInfoList = adapterInfoList->Next; // Progress through
        } while(adapterInfoList); // Terminate if last adapter

        ZBinary bin;
        bin.write(adapterInfo->Address, 6);
        delete[] adapterInfo;
        return bin;
    }
#else
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) { /* handle error*/ };

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { /* handle error */ }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for(; it != end; ++it){
        strcpy(ifr.ifr_name, it->ifr_name);
        if(ioctl(sock, SIOCGIFFLAGS, &ifr) == 0){
            if(!(ifr.ifr_flags & IFF_LOOPBACK)){ // skip loopback
                if(ioctl(sock, SIOCGIFHWADDR, &ifr) == 0){
                    unsigned char mac_address[6];
                    memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
                    ArZ mac;
                    for(zu64 i = 0; i < 6; ++i)
                        mac.push(ZString::ItoS(mac_address[i], 16, 2));
                    LOG(ZString::compound(mac, ":") << " " << ifr.ifr_name);

                    ZBinary bin(mac_address, 6);
                    return bin;
                }
            }
        } else {
            /* handle error */
        }
    }

    unsigned char mac_address[6];
    if(success)
        memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
#endif

    // Otherwise, generate random 6 bytes
    ZBinary rand;
    rand.resize(6);
    // TODO: Randomness here
    rand[0] |= 0x01; // Avoid collision with IEEE 802 MAC Addresses
    return rand;
}

ZUID::ZUID(){
    for(zu8 i = 0; i < 16; ++i)
        _id.octets[i] = 0;
//    memset(_id.octets, 0, 6);

#if PLATFORM == WINDOWS
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    FILETIME filetime;
    SystemTimeToFileTime(&systime, &filetime);
    LOG(filetime.dwHighDateTime << " " << filetime.dwLowDateTime);
#endif

    ZBinary mac = getMACAddress();
    mac.read(_id.octets + 10, 6);

    //clock_t clicks = clock();
    //time_t rawtime = time(NULL);
    //tm time = gmtime(rawtime);

}

ZUID::ZUID(ZString str){
    str.replace("-", "");
    if(str.size() == 32 && str.isInteger(16)){
        for(zu8 i = 0; i < 16; ++i)
            _id.octets[i] = (zu8)ZString::substr(str, i*2, 2).tozu64(16);
    } else {
        for(zu8 i = 0; i < 16; ++i)
            _id.octets[i] = 0;
    }
}

bool ZUID::operator==(const ZUID &uid){
    for(zu8 i = 0; i < 16; ++i)
        if(_id.octets[i] != uid._id.octets[i])
            return false;
    return true;
}


ZString ZUID::str() const {
    ZString uid;
    for(zu8 i = 0; i < 16; ++i)
        uid += ZString::ItoS(_id.octets[i], 16, 2);
    uid.insert(8, "-");
    uid.insert(8 + 1 + 4, "-");
    uid.insert(8 + 1 + 4 + 1 + 4, "-");
    uid.insert(8 + 1 + 4 + 1 + 4 + 1 + 4, "-");
    return uid;
}

}
