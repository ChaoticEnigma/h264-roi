/*******************************************************************************
**                                  LibChaos                                  **
**                                  zjson.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZJSON_H
#define ZJSON_H

#include "zstring.h"
#include "zmap.h"

namespace LibChaos {

//! JSON (JavaScript Object Notation)
class ZJSON {
public:
    enum jsontype {
        UNDEFINED = 0,
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
    };
public:
    ZJSON();
    ZJSON(ZString str);
    ZJSON(AsArZ assoc);

    ZJSON &operator=(ZString str);

    static bool validJSON(ZString str);
    bool isValid();

    bool decode(ZString str, zu64 *position = nullptr);

    ZString encode();

    AsArZ toZAssoc();
private:
    ZString json;
    union JSONValue {
        ZJSON object;
        ZArray<ZJSON> array;
        ZString string;
        ZString number;
    } data;
};

}

#endif // ZJSON_H
