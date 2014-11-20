#include "test.h"
#include "zbinary.h"

int binary_block(){
    ZBinary bin1({'A','B','C','D','E','F'});
    LOG(ZString(bin1.printable().asChar()));
    LOG(ZString(bin1.getSub(0, 2).printable().asChar()));
    LOG(ZString(bin1.getSub(2, 2).printable().asChar()));
    LOG(ZString(bin1.getSub(4, 6).printable().asChar()));
    LOG(ZString(bin1.getSub(5, 4).printable().asChar()));
    LOG(ZString(bin1.getSub(6, 2).printable().asChar()));

    ZBinary bin2({'A','0','B','C','D','2','2','E','F'});
    LOG(ZString(bin2.printable().asChar()));
    zu64 pos = bin2.findFirst({'2','2'});
    LOG(pos);
    LOG(ZString(bin2.getSub(0, pos).printable().asChar()));

    return 0;
}
