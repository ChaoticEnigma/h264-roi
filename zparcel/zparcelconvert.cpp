#include "zparcelconvert.h"
#include "zuid.h"

namespace LibChaos {
namespace ZParcelConvert {

using namespace ZParcelTypes;

ZBinary toFileFormat(fieldtype type, ZString str){
    switch(type){
        case unsignedintfield:
            return toFile64Bits(str.tozu64());
            break;
        case signedintfield:
            return toFile64Bits(str.tozu64()); // TODO: make this signed
            break;
        case zuidfield:
            return ZBinary(ZUID(str).getID(), 16);
            break;
        case stringfield:
            return ZBinary((const zu8 *)str.cc(), str.size());
            break;
        case filefield:
            return ZBinary((const zu8 *)str.cc(), str.size());
            break;
    }
    return ZBinary();
}

ZBinary toFile8Bits(zu8 num){
    ZBinary bin(1);
    bin[0] = num;
    return bin;
}
ZBinary toFile16Bits(zu16 num){
    ZBinary bin(2);
    bin[0] = (zu8)((num >> 8) & 0xFF);
    bin[1] = (zu8)((num)      & 0xFF);
    return bin;
}
ZBinary toFile32Bits(zu32 num){
    ZBinary bin(4);
    bin[0] = (zu8)((num >> 24) & 0xFF);
    bin[1] = (zu8)((num >> 16) & 0xFF);
    bin[2] = (zu8)((num >> 8)  & 0xFF);
    bin[3] = (zu8)((num)       & 0xFF);
    return bin;
}
ZBinary toFile64Bits(zu64 num){
    ZBinary bin(8);
    bin[0] = (zu8)((num >> 56) & 0xFF);
    bin[1] = (zu8)((num >> 48) & 0xFF);
    bin[2] = (zu8)((num >> 40) & 0xFF);
    bin[3] = (zu8)((num >> 32) & 0xFF);
    bin[4] = (zu8)((num >> 24) & 0xFF);
    bin[5] = (zu8)((num >> 16) & 0xFF);
    bin[6] = (zu8)((num >> 8)  & 0xFF);
    bin[7] = (zu8)((num)       & 0xFF);
    return bin;
}


zu8 fromFile8Bits(ZBinary num){
    return num[0];
}
zu16 fromFile16Bits(ZBinary num){
    return (zu16)(num[0] << 8) + (zu16)num[1];
}
zu32 fromFile32Bits(ZBinary num){
    return (zu32)(num[0] << 24) + (zu32)(num[1] << 16) + (zu32)(num[2] << 8) + (zu32)num[3];
}
zu64 fromFile64Bits(ZBinary num){
    return ((zu64)num[0] << 56) + ((zu64)num[1] << 48) + ((zu64)num[2] << 40) + ((zu64)num[3] << 32) +
           ((zu64)num[4] << 24) + ((zu64)num[5] << 16) + ((zu64)num[6] << 8) + (zu64)num[7];
}

}
}
