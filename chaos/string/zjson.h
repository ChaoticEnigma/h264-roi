/*******************************************************************************
**                                  LibChaos                                  **
**                                  zjson.h                                   **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZJSON_H
#define ZJSON_H

#include "zstring.h"
#include "zassoc.h"

namespace LibChaos {

//class ZJSON {
class ZJSON : public AsArZ {
public:
    ZJSON();
    ZJSON(ZString str);
    ZJSON(AsArZ assoc);

    ZJSON &operator=(ZString str);

    static bool validJSON(ZString str);
    bool isValid();

    static ZJSON fromJSON(ZString str);
    ZJSON &decode(ZString str);

    ZString encode();

    AsArZ toZAssoc();
private:
    ZString json;
    //union JSONValue {
    //    ZString str;
    //    ZJSON json;
    //} data;
};

}

#endif // ZJSON_H
