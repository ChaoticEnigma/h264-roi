/*******************************************************************************
**                                  LibChaos                                  **
**                                  zuid.cpp                                  **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zuid.h"
#include "zlog.h"
#include "zrandom.h"
#include "zmutex.h"

#include <time.h>

#if PLATFORM == WINDOWS || PLATFORM == CYGWIN
    #define ZUID_WINAPI
#endif

#ifdef ZUID_WINAPI
    #include <winsock2.h>
    #include <windows.h>
    #include <iptypes.h>
    #include <iphlpapi.h>
#else
    #include <unistd.h>
    #include <sys/time.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <ifaddrs.h>
    #include <net/if.h>
    //#include <net/if_dl.h>
    #include <netinet/in.h>
#endif

namespace LibChaos {

ZMutex zuidlock;
zu64 prevtime;
zu16 prevclock = 0;
ZBinary prevmac;

/*! Generates an RFC 4122 compliant UUID of \a type.
 *  Default is time-based UUID (type 1).
 */
ZUID::ZUID(uuidtype type){
    if(type == NIL){
        // Nil UUID
        for(zu8 i = 0; i < 16; ++i){
            _id_octets[i] = 0;
        }

    } else if(type == TIME){
        // Time-Clock-MAC Version 1 UUID
        zuidlock.lock();

        // Get the time
        zu64 utctime = getTimestamp();
        zu32 utchi = (utctime >> 32);
        zu32 utclo = (utctime & 0xFFFFFFFF);

        // Write time
        _id_octets[0] = (zu8)((utclo >> 24) & 0xFF); // time_lo
        _id_octets[1] = (zu8)((utclo >> 16) & 0xFF);
        _id_octets[2] = (zu8)((utclo >> 8)  & 0xFF);
        _id_octets[3] = (zu8) (utclo        & 0xFF);
        _id_octets[4] = (zu8)((utchi >> 8)  & 0xFF); // time_med
        _id_octets[5] = (zu8) (utchi        & 0xFF);
        _id_octets[6] = (zu8)((utchi >> 24) & 0xFF); // time_hi
        _id_octets[7] = (zu8)((utchi >> 16) & 0xFF);

        // Write version
        _id_octets[6] &= 0x0F; // 0b00001111 // Clear the 4 highest bits of the 7th byte
        _id_octets[6] |= 0x10; // 0b00010000 // Insert UUID version 1

        zu16 clock;
        ZBinary mac = getMACAddress();

        // Test previous values
        if(prevclock == 0){
            ZRandom randgen;
            ZBinary randomdat = randgen.generate(2);
            clock = (randomdat[0] << 8) | randomdat[1];
//            random.read(_id_octets + 8, 2);
        } else {
            clock = prevclock;
            if(prevtime >= utctime || prevmac != mac){
                ++clock;
            }
        }

        // Write clock
        _id_octets[8] = (zu8)((clock >> 8)  & 0xFF);
        _id_octets[9] = (zu8) (clock        & 0xFF);

        // Write variant
        _id_octets[8] &= 0x3F; // 0b00111111 // Clear the 2 highest bits of the 9th byte
        _id_octets[8] |= 0x80; // 0b10000000 // Insert UUID variant "10x"

        // Write MAC
        mac.read(_id_octets + 10, 6);

        // Save these values
        prevtime = utctime;
        prevclock = clock;
        prevmac = mac;

        zuidlock.unlock();

    } else if(type == RANDOM){
        // Randomly generated Version 4 UUID
        ZRandom randgen;
        ZBinary random = randgen.generate(16);
        random.read(_id_octets, 16);

        _id_octets[6] &= 0x0F; // 0b00001111 // Clear the 4 highest bits of the 7th byte
        _id_octets[6] |= 0x40; // 0b01000000 // Insert UUID version 4

        _id_octets[8] &= 0x3F; // 0b00111111 // Clear the 2 highest bits of the 9th byte
        _id_octets[8] |= 0x80; // 0b10000000 // Insert UUID variant "10x"

    } else {
        ELOG("Invalid ZUID type");
    }
}

ZUID::ZUID(ZString str){
    str.replace("-", "");
    str.replace(":", "");
    if(str.size() == 32 && str.isInteger(16)){
        for(zu64 i = 0; i < 16; ++i)
            _id_octets[i] = (zu8)ZString::substr(str, i*2, 2).tozu64(16);
    } else {
        for(zu8 i = 0; i < 16; ++i)
            _id_octets[i] = 0;
    }
}

bool ZUID::operator==(const ZUID &uid){
    for(zu8 i = 0; i < 16; ++i)
        if(_id_octets[i] != uid._id_octets[i])
            return false;
    return true;
}

ZUID::uuidtype ZUID::getType() const {
    zu8 type = _id_octets[6] & 0xF0 >> 4;
    switch(type){
        case 1:
            return TIME;
        case 4:
            return RANDOM;
        default:
            return UNKNOWN;
    }
}

ZString ZUID::str() const {
    ZString uid;
    for(zu8 i = 0; i < 16; ++i)
        uid += ZString::ItoS(_id_octets[i], 16, 2);
    uid.insert(8, "-");
    uid.insert(8 + 1 + 4, "-");
    uid.insert(8 + 1 + 4 + 1 + 4, "-");
    uid.insert(8 + 1 + 4 + 1 + 4 + 1 + 4, "-");
    return uid;
}

ZBinary ZUID::bin() const {
    return ZBinary(_id_octets, 16);
}

zu64 ZUID::getTimestamp(){
#ifdef ZUID_WINAPI
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    FILETIME filetime;
    SystemTimeToFileTime(&systime, &filetime);
    // Time in 100-nanosecond intervals
    zu64 mstime = ((zu64)filetime.dwHighDateTime << 32) | filetime.dwLowDateTime;
    // Microsoft UTC starts January 1, 1601, 00:00:00.0000000
    // We need UTC since October 15, 1582, 00:00:00.0000000
    // Add 18 years + 17 days in Oct + 30 days in Nov + 31 days in Dec + 5 leap days, to seconds, to 100 nanosecond interval
    zu64 utctime = mstime + ((zu64)((18*365)+17+30+31+5) * (60*60*24) * (1000*1000*10));
#else
    timeval tv;
    gettimeofday(&tv, NULL);
    // Time in microseconds
    zu64 tvtime = (zu64)((tv.tv_sec * 1000 * 1000) + tv.tv_usec);
    // POSIX UTC start January 1, 1970, 00:00:00.000000
    // We need UTC since October 15, 1582, 00:00:00.0000000
    // Add 387 years + 17 days in Oct + 30 days in Nov + 31 days in Dec + 94 leap days, to seconds, to 100 nanosecond interval
    zu64 utctime = (tvtime * 10) + ((zu64)((387*365)+17+30+31+5) * (60*60*24) * (1000*1000*10));
#endif
    return utctime;
}

ZList<ZBinary> ZUID::getMACAddresses(){
    ZList<ZBinary> maclist;
#ifdef ZUID_WINAPI
    ULONG addrslen = sizeof(IP_ADAPTER_ADDRESSES);
    IP_ADAPTER_ADDRESSES *addrs = (IP_ADAPTER_ADDRESSES *)new zbyte[addrslen];
    ULONG flags = GAA_FLAG_INCLUDE_ALL_INTERFACES;
    ULONG ret = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, addrs, &addrslen);
    if(ret == ERROR_BUFFER_OVERFLOW){
        delete[] addrs;
        addrs = (IP_ADAPTER_ADDRESSES *)new zbyte[addrslen];
        ret = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, addrs, &addrslen);
    }
    if(ret == NO_ERROR){
        IP_ADAPTER_ADDRESSES *addr = addrs;
        while(addrs != NULL){
            if(addr->PhysicalAddressLength == 6){
                ZBinary mac(addr->PhysicalAddress, 6);
                ZString macstr;
                for(zu64 i = 0 ; i < mac.size(); ++i)
                    macstr += ZString::ItoS(mac[i], 16, 2) += ":";
                macstr.substr(0, macstr.size()-1);
                LOG(macstr);
            }
            addr = addr->Next;
        }
    }
    delete[] addrs;
    LOG("done");

    /*
    PIP_ADAPTER_INFO adapterInfo;
    ULONG bufLen = sizeof(IP_ADAPTER_INFO);
    adapterInfo = new IP_ADAPTER_INFO[1];

    // Get number of adapters and list of adapter info
    DWORD status = GetAdaptersInfo(adapterInfo, &bufLen);
    if(status == ERROR_BUFFER_OVERFLOW){
        // Make larger list for adapters
        delete[] adapterInfo;
        adapterInfo = new IP_ADAPTER_INFO[bufLen];
        status = GetAdaptersInfo(adapterInfo, &bufLen);
    }

    if(status == NO_ERROR){
        // Get first acceptable MAC from list
        PIP_ADAPTER_INFO adapterInfoList = adapterInfo;
        while(adapterInfoList != NULL){
            if(validMAC(adapterInfoList->Address)){
                maclist.push(ZBinary(adapterInfoList->Address, 6));
                delete[] adapterInfo;
            }
            adapterInfoList = adapterInfoList->Next;
        }
    }
    delete[] adapterInfo;
    */

#elif PLATFORM == MACOSX

    ifaddrs *iflist = NULL;
    // Get list of interfaces and addresses
    int r = getifaddrs(&iflist);
    if(r == 0 && iflist != NULL){
        ifaddrs *current = iflist;
        // Walk linked list
        while(current != NULL){
            // Look for an interface with a hardware address
            if((current->ifa_addr != NULL) && (current->ifa_addr->sa_family == AF_LINK)){
                sockaddr_dl *sockdl = (sockaddr_dl *)current->ifa_addr;
                uint8_t *mac = reinterpret_cast<uint8_t*>(LLADDR(sockdl));
                maclist.push(ZBinary(mac, 6));
            }
            current = current->ifa_next;
        }
    }

#else

    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    unsigned char mac_address[6];

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(sock != -1){
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        if(ioctl(sock, SIOCGIFCONF, &ifc) != -1){
            struct ifreq *it = ifc.ifc_req;
            const struct ifreq *end = it + ((unsigned long)ifc.ifc_len / sizeof(struct ifreq));
            for(; it != end; ++it){
                strcpy(ifr.ifr_name, it->ifr_name);
                DLOG(ifr.ifr_name);
                // Get interface flags
                if(ioctl(sock, SIOCGIFFLAGS, &ifr) == 0){
                    // Skip loopback interface
                    if(!(ifr.ifr_flags & IFF_LOOPBACK)){
                        // Get hardware address
                        // TODO: FreeBSD MAC Address
#if PLATFORM == FREEBSD
                        if(ioctl(sock, SIOCGIFMAC, &ifr) == 0){
                            memcpy(mac_address, ifr.ifr_mac.sa_data, 6);
#else
                        if(ioctl(sock, SIOCGIFHWADDR, &ifr) == 0){
                            memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
#endif
                            if(validMAC(mac_address)){
                                maclist.push(ZBinary(mac_address, 6));
                            } else {
                                DLOG("in2valid mac");
                            }
                        } else {
                            DLOG("failed to get mac");
                        }
                    } else {
                        DLOG("skip loopback");
                    }
                } else {
                    // Try next interface
                    DLOG("failed to get if flags");
                }
            }
        } else {
            // ioctl failed
            DLOG("failed to get if config");
        }
    } else {
        // socket failed
        DLOG("failed to open socket");
    }

#endif
    return maclist;
}

ZBinary ZUID::getMACAddress(){
#ifdef ZUID_WINAPI
    PIP_ADAPTER_INFO adapterInfo;
    ULONG bufLen = sizeof(IP_ADAPTER_INFO);
    adapterInfo = new IP_ADAPTER_INFO[1];

    // Get number of adapters and list of adapter info
    DWORD status = GetAdaptersInfo(adapterInfo, &bufLen);
    if(status == ERROR_BUFFER_OVERFLOW){
        // Make larger list for adapters
        delete[] adapterInfo;
        adapterInfo = new IP_ADAPTER_INFO[bufLen];
        status = GetAdaptersInfo(adapterInfo, &bufLen);
    }

    if(status == NO_ERROR){
        // Get first acceptable MAC from list
        PIP_ADAPTER_INFO adapterInfoList = adapterInfo;
        while(adapterInfoList != NULL){
            if(validMAC(adapterInfoList->Address)){
                ZBinary bin(adapterInfoList->Address, 6);
                delete[] adapterInfo;
                return bin;
            }
            adapterInfoList = adapterInfoList->Next;
        }
    }
    delete[] adapterInfo;

#elif PLATFORM == MACOSX

    ifaddrs *iflist = NULL;
    // Get list of interfaces and addresses
    int r = getifaddrs(&iflist);
    if(r == 0 && iflist != NULL){
        ifaddrs *current = iflist;
        // Walk linked list
        while(current != NULL){
            // Look for an interface with a hardware address
            if((current->ifa_addr != NULL) && (current->ifa_addr->sa_family == AF_LINK)){
                sockaddr_dl *sockdl = (sockaddr_dl *)current->ifa_addr;
                uint8_t *mac = reinterpret_cast<uint8_t*>(LLADDR(sockdl));
                // Get first valid MAC
                if(validMAC(mac)){
                    ZBinary bin(mac, 6);
                    return bin;
                }
            }
            current = current->ifa_next;
        }
    }

#else
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    unsigned char mac_address[6];

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(sock != -1){
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        if(ioctl(sock, SIOCGIFCONF, &ifc) != -1){
            struct ifreq *it = ifc.ifc_req;
            const struct ifreq *end = it + ((unsigned long)ifc.ifc_len / sizeof(struct ifreq));
            for(; it != end; ++it){
                strcpy(ifr.ifr_name, it->ifr_name);
                DLOG(ifr.ifr_name);
                // Get interface flags
                if(ioctl(sock, SIOCGIFFLAGS, &ifr) == 0){
                    // Skip loopback interface
                    if(!(ifr.ifr_flags & IFF_LOOPBACK)){
                        // Get hardware address
                        // TODO: FreeBSD MAC Address
#if PLATFORM == FREEBSD
                        if(ioctl(sock, SIOCGIFMAC, &ifr) == 0){
                            memcpy(mac_address, ifr.ifr_mac.sa_data, 6);
#else
                        if(ioctl(sock, SIOCGIFHWADDR, &ifr) == 0){
                            memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
#endif
                            if(validMAC(mac_address)){
                                ZBinary bin(mac_address, 6);
                                return bin;
                            } else {
                                DLOG("in2valid mac");
                            }
                        } else {
                            DLOG("failed to get mac");
                        }
                    } else {
                        DLOG("skip loopback");
                    }
                } else {
                    // Try next interface
                    DLOG("failed to get if flags");
                }
            }
        } else {
            // ioctl failed
            DLOG("failed to get if config");
        }
    } else {
        // socket failed
        DLOG("failed to open socket");
    }
#endif

    // Otherwise, generate random 6 bytes
    ZRandom rand;
    ZBinary addr = rand.generate(6);
    addr[0] |= 0x02; // Mark as locally administered to avoid collision with real IEEE 802 MAC Addresses
    return addr;
}

bool ZUID::validMAC(const zoctet *addr){
    if(addr == NULL) return false;
    // Zero address
    if(addr[0] == 0 && addr[1] == 0 && addr[2] == 0 && addr[3] == 0 && addr[4] == 0 && addr[5] == 0) return false;
    // Locally administered address
    if(addr[0] & 0x02) return false;
    return true;
}

}
