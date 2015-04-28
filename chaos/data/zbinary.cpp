#include "zbinary.h"

namespace LibChaos {

void ZBinary::enczu8(zbyte *bin, zu8 num){
    bin[0] = (zbyte)num;
}
void ZBinary::enczu16(zbyte *bin, zu16 num){
    bin[0] = (zbyte)((num >> 8) & 0xFF);
    bin[1] = (zbyte)((num)      & 0xFF);
}
void ZBinary::enczu32(zbyte *bin, zu32 num){
    bin[0] = (zbyte)((num >> 24) & 0xFF);
    bin[1] = (zbyte)((num >> 16) & 0xFF);
    bin[2] = (zbyte)((num >> 8)  & 0xFF);
    bin[3] = (zbyte)((num)       & 0xFF);
}
void ZBinary::enczu64(zbyte *bin, zu64 num){
    bin[0] = (zbyte)((num >> 56) & 0xFF);
    bin[1] = (zbyte)((num >> 48) & 0xFF);
    bin[2] = (zbyte)((num >> 40) & 0xFF);
    bin[3] = (zbyte)((num >> 32) & 0xFF);
    bin[4] = (zbyte)((num >> 24) & 0xFF);
    bin[5] = (zbyte)((num >> 16) & 0xFF);
    bin[6] = (zbyte)((num >> 8)  & 0xFF);
    bin[7] = (zbyte)((num)       & 0xFF);
}

zu8 ZBinary::deczu8(const zbyte *bin){
    return (zu8)bin[0];
}
zu16 ZBinary::deczu16(const zbyte *bin){
    return ((zu16)bin[0] << 8) &
           ((zu16)bin[1]);
}
zu32 ZBinary::deczu32(const zbyte *bin){
    return ((zu32)bin[0] << 24) &
           ((zu32)bin[1] << 16) &
           ((zu32)bin[2] << 8)  &
           ((zu32)bin[3]);
}
zu64 ZBinary::deczu64(const zbyte *bin){
    return ((zu64)bin[0] << 56) &
           ((zu64)bin[1] << 48) &
           ((zu64)bin[2] << 40) &
           ((zu64)bin[3] << 32) &
           ((zu64)bin[4] << 24) &
           ((zu64)bin[5] << 16) &
           ((zu64)bin[6] << 8)  &
           ((zu64)bin[7]);
}

}
