#ifndef ZPARCELCONVERT_H
#define ZPARCELCONVERT_H

#include "ztypes.h"
#include "zbinary.h"
#include "zstring.h"

#include "zparceltypes.h"

namespace LibChaos {
namespace ZParcelConvert {

    ZBinary toFileFormat(ZParcelTypes::fieldtype type, ZString str);

    ZBinary toFile8Bits(zu8 num);
    ZBinary toFile16Bits(zu16 num);
    ZBinary toFile32Bits(zu32 num);
    ZBinary toFile64Bits(zu64 num);
    zu8 fromFile8Bits(ZBinary num);
    zu16 fromFile16Bits(ZBinary num);
    zu32 fromFile32Bits(ZBinary num);
    zu64 fromFile64Bits(ZBinary num);

}
}

#endif // ZPARCELCONVERT_H
