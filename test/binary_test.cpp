#include "test.h"
#include "zbinary.h"

int binary_block(){
    ZBinary bin1({'A','B','C','D','E','F'});
    LOG(ZString(bin1.printable().storage()));
    LOG(ZString(bin1.getSub(0, 2).printable().storage()));
    LOG(ZString(bin1.getSub(2, 2).printable().storage()));
    LOG(ZString(bin1.getSub(4, 6).printable().storage()));
    LOG(ZString(bin1.getSub(5, 4).printable().storage()));
    LOG(ZString(bin1.getSub(6, 2).printable().storage()));

    ZBinary bin2({'A','0','B','C','D','2','2','E','F'});
    LOG(ZString(bin2.printable().storage()));
    zu64 pos = bin2.findFirst({'2','2'});
    LOG(pos);
    LOG(ZString(bin2.getSub(0, pos).printable().storage()));

    return 0;
}
