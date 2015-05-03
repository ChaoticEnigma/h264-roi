/*******************************************************************************
**                                  LibChaos                                  **
**                                  zuid.cpp                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zuid.h"
#include "zlog.h"
#include "zrandom.h"
#include "zmutex.h"

#include <time.h>

#if PLATFORM == WINDOWS
    #include <windows.h>
    #include <iptypes.h>
    #include <iphlpapi.h>
#else
    #include <time.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <unistd.h>
    #include <netinet/in.h>
#endif

namespace LibChaos {

ZMutex zuidlock;
zu64 prevtime;
zu16 prevclock = 0;
ZBinary prevmac;

ZBinary getMACAddress();

ZUID::ZUID(uuidtype type){
    if(type == nil){
        // Nil UUID
        for(zu8 i = 0; i < 16; ++i){
            _id_octets[i] = 0;
        }

    } else if(type == time){
        // Time-Clock-MAC Version 1 UUID
        zuidlock.lock();

        // Get the time
#if PLATFORM == WINDOWS
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
        zu64 tvtime = (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
        // POSIX UTC start January 1, 1970, 00:00:00.000000
        // We need UTC since October 15, 1582, 00:00:00.0000000
        // Add 387 years + 17 days in Oct + 30 days in Nov + 31 days in Dec + 94 leap days, to seconds, to 100 nanosecond interval
        zu64 utctime = (tvime * 10) + ((zu64)((387*365)+17+30+31+5) * (60*60*24) * (1000*1000*10));
#endif

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
            ZBinary random = randgen.generate(2);
            clock = (random[0] << 8) | random[1];
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

    } else if(type == random){
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
            return time;
        case 4:
            return random;
        default:
            return unknown;
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
//        PIP_ADAPTER_INFO adapterInfoList = adapterInfo;
//        do {
//            ArZ mac;
//            for(zu64 i = 0; i < 6; ++i)
//                mac.push(ZString::ItoS(adapterInfoList->Address[i], 16, 2));
//            LOG(ZString::compound(mac, ":") << " " << adapterInfoList->Description);
//            adapterInfoList = adapterInfoList->Next; // Progress through
//        } while(adapterInfoList); // Terminate if last adapter

        ZBinary bin;
        bin.write(adapterInfo->Address, 6);
        delete[] adapterInfo;
        return bin;
    }
#elif PLATFORM == MACOSX

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

}
