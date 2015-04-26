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

ZParcel::ZParcel() : _parser(nullptr), _version(versionunknown){

}

ZParcel::~ZParcel(){
    close();
}

void ZParcel::create(ZPath path){
    _file.open(file, ZFile::modereadwrite);
    _parser = new ZParcel4Parser(&_file);
    _version = version4;
    _parser->create();
}

bool ZParcel::open(ZPath file){
    _file.open(file, ZFile::modereadwrite);
    _parser = new ZParcel4Parser(&_file);
    return _parser->open();
}

void ZParcel::close(){
    delete _parser;
    _file.close();
}

void ZParcel::addRecords(ZList<ZParcel::Field> fields){

}

void ZParcel::addUintRecord(fieldid field, zu64 num){
    ZBinary bin = toFile64Bits(num);
}

void ZParcel::addSintRecord(fieldid field, zs64 num){
    ZBinary bin = toFile64Bits((zu64)num);
}

void ZParcel::addZUIDRecord(fieldid field, ZUID uid){
    ZBinary bin = ZBinary(uid.raw(), 16);
}

void ZParcel::addFloatRecord(fieldid field, double flt){

}

void ZParcel::addStringRecord(fieldid field, ZString str){
    ZBinary bin = ZBinary((const zbyte *)str.cc(), str.size());
}

void ZParcel::addBinaryRecord(fieldid field, ZBinary bin){

}

void ZParcel::addFileRecord(fieldid field, ZPath file){
    ZString str = file.str();
    ZBinary bin = ZBinary((const zbyte *)str.cc(), str.size());
}

ZParcel::fieldtype ZParcel::fieldType(ZString name){
    return nametofield[name];
}

ZString ZParcel::fieldName(fieldtype type){
    return fieldtoname[type];
}


ZBinary ZParcel::toFile8Bits(zu8 num){
    ZBinary bin(1);
    bin[0] = num;
    return bin;
}
ZBinary ZParcel::toFile16Bits(zu16 num){
    ZBinary bin(2);
    bin[0] = (zu8)((num >> 8) & 0xFF);
    bin[1] = (zu8)((num)      & 0xFF);
    return bin;
}
ZBinary ZParcel::toFile32Bits(zu32 num){
    ZBinary bin(4);
    bin[0] = (zu8)((num >> 24) & 0xFF);
    bin[1] = (zu8)((num >> 16) & 0xFF);
    bin[2] = (zu8)((num >> 8)  & 0xFF);
    bin[3] = (zu8)((num)       & 0xFF);
    return bin;
}
ZBinary ZParcel::toFile64Bits(zu64 num){
    ZBinary bin(8);
    bin[0] = (zu8)((num >> 56) & 0xFF);
    bin[1] = (zu8)((num >> 48) & 0xFF);
    bin[2] = (zu8)((num >> 40) & 0xFF);
    bin[3] = (zu8)((num >> 32) & 0xFF);
    bin[4] = (zu8)((num >> 24) & 0xFF);
    bin[5] = (zu8)((num >> 16) & 0xFF);
    bin[6] = (zu8)((num >> 8)  & 0xFF);
    bin[7] = (zu8)((num)       & 0xFF);
    return bin;
}

zu8 ZParcel::fromFile8Bits(ZBinary num){
    return num[0];
}
zu16 ZParcel::fromFile16Bits(ZBinary num){
    return (zu16)(num[0] << 8) + (zu16)num[1];
}
zu32 ZParcel::fromFile32Bits(ZBinary num){
    return (zu32)(num[0] << 24) + (zu32)(num[1] << 16) + (zu32)(num[2] << 8) + (zu32)num[3];
}
zu64 ZParcel::fromFile64Bits(ZBinary num){
    return ((zu64)num[0] << 56) + ((zu64)num[1] << 48) + ((zu64)num[2] << 40) + ((zu64)num[3] << 32) +
           ((zu64)num[4] << 24) + ((zu64)num[5] << 16) + ((zu64)num[6] << 8) + (zu64)num[7];
}

}
