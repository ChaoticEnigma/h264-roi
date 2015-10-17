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
        UNDEF = 0,
        OBJECT,
        ARRAY,
        STRING,
        NUMBER,
        BOOLEAN,
        NULLVAL,
    };
public:
    ZJSON();
    ZJSON(ZString str);

    //! Copy constructor.
    ZJSON(const ZJSON &other);

    //! String assignment overload.
    ZJSON &operator=(ZString str);

    static bool validJSON(ZString str);
    bool isValid();

    //! Decode JSON string.
    bool decode(ZString str, zu64 *position = nullptr);

    //! Encode JSON string.
    ZString encode();

private:
    //! JSON type.
    jsontype type;

    //! Decoded JSON data.
    union JSONValue {
        JSONValue();

        ZMap<ZString, ZJSON> object;
        ZArray<ZJSON> array;
        ZString string;
        ZString number;
        bool boolean;
    } data;
};

}

#endif // ZJSON_H
