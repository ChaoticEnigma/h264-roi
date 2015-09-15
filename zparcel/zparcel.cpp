#include "zparcel.h"

#include "zlog.h"

namespace LibChaos {

static const ZMap<ZString, ZParcel::fieldtype> nametotype = {
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
    { "bin",        ZParcel::blobfield },
    { "blob",       ZParcel::blobfield },

    { "file",       ZParcel::filefield },
};

static const ZMap<ZParcel::fieldtype, ZString> typetoname = {
    { ZParcel::nullfield,   "null" },
    { ZParcel::uintfield,   "unsigned int" },
    { ZParcel::sintfield,   "signed int" },
    { ZParcel::floatfield,  "float" },
    { ZParcel::zuidfield,   "zuid" },
    { ZParcel::stringfield, "string" },
    { ZParcel::filefield,   "file" },
    { ZParcel::blobfield,   "binary" },
};

ZParcel::ZParcel() : _parser(nullptr), _version(versionunknown){

}

ZParcel::~ZParcel(){
    close();
}

void ZParcel::create(ZPath path){
    _file.open(path, ZFile::READWRITE);
    _parser = new ZParcel4Parser(&_file);
    _version = version4;
    _parser->create();
}

bool ZParcel::open(ZPath file){
    _file.open(file, ZFile::READWRITE);
    _parser = new ZParcel4Parser(&_file);
    _parser->open();
    return true;
}

void ZParcel::close(){
    delete _parser;
    _file.close();
}

void ZParcel::addRecord(ZParcelRecord record){
    addRecords({ fields });
}

void ZParcel::addRecords(ZList<ZParcelRecord> records){
    LOG("Adding " << records.size() << " records");

}

ZParcel::fieldid ZParcel::getFieldId(ZString name){
    return 0;
}

ZParcel::fieldtype ZParcel::typeType(ZString name){
    return nametotype[name];
}

ZString ZParcel::typeName(fieldtype type){
    return typetoname[type];
}

}
