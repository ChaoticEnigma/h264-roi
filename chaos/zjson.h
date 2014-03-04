#ifndef ZJSON_H
#define ZJSON_H

#include "zstring.h"
#include "zassoc.h"

namespace LibChaos {

class ZJSON {
public:
    ZJSON();
    ZJSON(ZString);
    ZJSON(AsArZ);

    ZJSON &operator=(ZString);

    static bool validJSON(ZString);
    bool isValid();

    static ZJSON fromJSON(ZString);
    ZJSON &decode(ZString);

    ZString encode();

    AsArZ toZAssoc();
private:
    union JSONValue {
        ZString str;
        ZJSON json;
    } data;
};

}

#endif // ZJSON_H
