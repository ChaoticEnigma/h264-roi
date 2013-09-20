#ifndef ZJSON_H
#define ZJSON_H

#include "zassoc.h"
#include "zstring.h"

namespace LibChaos {

class ZJSON : public AsArZ {
public:
    ZJSON();
    ZJSON(ZString);
    ZJSON(AsArZ);

    ZString toJSON();

    static bool validJSON(ZString);
    ZJSON fromJSON(ZString);

    AsArZ toZAssoc();
};

}

#endif // ZJSON_H
