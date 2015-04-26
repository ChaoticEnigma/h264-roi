#include "zparcel.h"

namespace LibChaos {

static const ZMap<ZString, ZParcel::fieldtype> nametofield = {
    { "null",       ZParcel::nullfield },

    { "uint",       ZParcel::unsignedintfield },

    { "sint",       ZParcel::signedintfield },
    { "int",        ZParcel::signedintfield },
    { "integer",    ZParcel::signedintfield },

    { "float",      ZParcel::floatfield },
    { "double",     ZParcel::floatfield },
    { "decimal",    ZParcel::floatfield },

    { "zuid",       ZParcel::zuidfield },
    { "uuid",       ZParcel::zuidfield },
    { "guid",       ZParcel::zuidfield },
    { "uid",        ZParcel::zuidfield },

    { "string",     ZParcel::stringfield },
    { "str",        ZParcel::stringfield },
    { "text",       ZParcel::stringfield },

    { "binary",     ZParcel::binaryfield },
    { "blob",       ZParcel::binaryfield },

    { "file",       ZParcel::filefield },
};

static const ZMap<ZParcel::fieldtype, ZString> fieldtoname = {
    { ZParcel::nullfield,        "null" },
    { ZParcel::unsignedintfield, "unsigned int" },
    { ZParcel::signedintfield,   "signed int" },
    { ZParcel::floatfield,       "float" },
    { ZParcel::zuidfield,        "zuid" },
    { ZParcel::stringfield,      "string" },
    { ZParcel::filefield,        "file" },
    { ZParcel::binaryfield,      "binary" },
};

ZParcel::ZParcel(){

}

ZParcel::~ZParcel(){

}

void ZParcel::create(ZPath path){

    return false;
}

bool ZParcel::open(ZPath file){

    return false;
}

void ZParcel::close(){

}

ZParcel::fieldtype ZParcel::fieldType(ZString name){
    return nametofield[name];
}

ZString ZParcel::fieldName(fieldtype type){
    return fieldtoname[type];
}

}
