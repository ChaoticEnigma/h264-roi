#include "zparcel.h"

namespace LibChaos {

static const ZMap<ZString, ZParcel::fieldtype> nametofield = {
    { "null",       ZParcel::nullfield },

    { "uint",       ZParcel::uintfield },

    { "sint",       ZParcel::sintfield },
    { "int",        ZParcel::sintfield },
    { "integer",    ZParcel::sintfield },

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

    { "binary",     ZParcel::blobfield },
    { "blob",       ZParcel::blobfield },

    { "file",       ZParcel::filefield },
};

static const ZMap<ZParcel::fieldtype, ZString> fieldtoname = {
    { ZParcel::nullfield,        "null" },
    { ZParcel::uintfield, "unsigned int" },
    { ZParcel::sintfield,   "signed int" },
    { ZParcel::floatfield,       "float" },
    { ZParcel::zuidfield,        "zuid" },
    { ZParcel::stringfield,      "string" },
    { ZParcel::filefield,        "file" },
    { ZParcel::blobfield,      "binary" },
};

ZParcel::ZParcel() : _parser(nullptr), _version(versionunknown){

}

ZParcel::~ZParcel(){
    close();
}

void ZParcel::create(ZPath path){
    _file.open(path, ZFile::modereadwrite);
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

void ZParcel::addBoolRecord(fieldid field, bool tf){
    _parser->addBoolRecord(field, tf);
}

void ZParcel::addUintRecord(fieldid field, zu64 num){
    _parser->addUintRecord(field, num);
}

void ZParcel::addSintRecord(fieldid field, zs64 num){
    _parser->addSintRecord(field, num);
}

void ZParcel::addZUIDRecord(fieldid field, ZUID uid){
    _parser->addZUIDRecord(field, uid);
}

void ZParcel::addFloatRecord(fieldid field, double flt){
    _parser->addFloatRecord(field, flt);
}

void ZParcel::addStringRecord(fieldid field, ZString str){
    _parser->addStringRecord(field, str);
}

void ZParcel::addBinaryRecord(fieldid field, ZBinary bin){
    _parser->addBinaryRecord(field, bin);
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

}
