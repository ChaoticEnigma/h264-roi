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

//! JSON (JavaScript Object Notation) container, decoder and encoder.
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
    ZJSON(jsontype type = OBJECT);
    ZJSON(ZString str);

    //! Copy constructor.
    ZJSON(const ZJSON &other);
    //! Assignment operator.
    ZJSON &operator=(const ZJSON &other);

    //! Encode JSON string.
    ZString encode();

    static bool validJSON(ZString str);
    bool isValid();

    //! String assignment decode overload.
    ZJSON &operator=(const ZString &str);
    //! Decode JSON string.
    bool decode(ZString str, zu64 *position = nullptr);

    //! Get type of this JSON object.
    jsontype type() const { return _type; }

    // Data accessors
    ZMap<ZString, ZJSON> &object();
    ZArray<ZJSON> &array();
    ZString &string();
    ZString &number();
    bool &boolean();

private:
    void initType(jsontype type);

private:
    //! JSON type.
    jsontype _type;

    //! Decoded JSON data.
#if COMPILER == MSVC
    struct JSONValue {
#else
    union JSONValue {
#endif
        JSONValue();
        ~JSONValue();

        ZMap<ZString, ZJSON> object;
        ZArray<ZJSON> array;
        ZString string;
        ZString number;
        bool boolean;
    } _data;
};

}

#endif // ZJSON_H
