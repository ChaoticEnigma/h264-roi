#include "test.h"
#include "zbinary.h"

int binary_block(){
    ZBinary bin1({'A','B','C','D','E','F'});
    LOG(bin1.printable().asChar());
    LOG(bin1.getSub(0, 2).printable().asChar());
    LOG(bin1.getSub(2, 2).printable().asChar());
    LOG(bin1.getSub(4, 6).printable().asChar());
    LOG(bin1.getSub(5, 4).printable().asChar());
    LOG(bin1.getSub(6, 2).printable().asChar());

    ZBinary bin2({'A','\n','B','C','D','\n','\n','E','F'});
    LOG(bin2.printable().asChar());
    zu64 pos = bin2.findFirst({'\n','\n'});
    LOG(pos);
    LOG(bin2.getSub(0, pos).printable().asChar());

    return 0;
}
