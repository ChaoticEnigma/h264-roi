#include "zparcel.h"

#include "zlog.h"

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
    _parser->open();
    return true;
}

void ZParcel::close(){
    delete _parser;
    _file.close();
}

void ZParcel::addRecord(ZMap<ZString, ZString> fields){
    addRecords({ fields });
}

void ZParcel::addRecords(ZList< ZMap<ZString, ZString> > records){
    LOG("Adding " << records.size() << " records");

}

void ZParcel::addBoolRecord(ZString field, bool tf){
    _parser->addBoolData(getFieldId(field), tf);
}

void ZParcel::addUintRecord(ZString field, zu64 num){
    _parser->addUintData(getFieldId(field), num);
}

void ZParcel::addSintRecord(ZString field, zs64 num){
    _parser->addSintData(getFieldId(field), num);
}

void ZParcel::addZUIDRecord(ZString field, ZUID uid){
    _parser->addZUIDData(getFieldId(field), uid);
}

void ZParcel::addFloatRecord(ZString field, double flt){
    _parser->addFloatData(getFieldId(field), flt);
}

void ZParcel::addStringRecord(ZString field, ZString str){
    _parser->addStringData(getFieldId(field), str);
}

void ZParcel::addBinaryRecord(ZString field, ZBinary bin){
    _parser->addBinaryData(getFieldId(field), bin);
}

void ZParcel::addFileRecord(ZString field, ZPath file){
    _parser->addFileData(getFieldId(field), file);
}

ZParcel::fieldid ZParcel::getFieldId(ZString name){
    return 0;
}

ZParcel::fieldtype ZParcel::fieldType(ZString name){
    return nametofield[name];
}

ZString ZParcel::fieldName(fieldtype type){
    return fieldtoname[type];
}

}
