#include "zparcel.h"

#include "zlog.h"

namespace LibChaos {

static const ZMap<ZParcel::fieldtype, ZString> typetoname = {
    { ZParcel::NULLFIELD,   "null" },
    { ZParcel::UINTFIELD,   "uint" },
    { ZParcel::SINTFIELD,   "sint" },
    { ZParcel::FLOATFIELD,  "float" },
    { ZParcel::ZUIDFIELD,   "zuid" },
    { ZParcel::STRINGFIELD, "string" },
    { ZParcel::FILEFIELD,   "file" },
    { ZParcel::BLOBFIELD,   "binary" },
};

ZParcel::ZParcel() : _parser(nullptr), _version(UNKNOWN){

}

ZParcel::~ZParcel(){
    close();
}

void ZParcel::create(ZPath path){
    _file.open(path, ZFile::READWRITE);
    _parser = new ZParcel4Parser(&_file);
    _version = VERSION1;
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

// /////////////////////////////////////////////////////////////////////////////

void ZParcel::storeNull(ZUID id){

}

void ZParcel::storeBool(ZUID id, bool bl){

}

void ZParcel::storeUint(ZUID id, zu64 num){

}

void ZParcel::storeSint(ZUID id, zs64 num){

}

void ZParcel::storeFloat(ZUID id, double num){

}

void ZParcel::storeZUID(ZUID id, ZUID uid){

}

void ZParcel::storeString(ZUID id, ZString str){

}

void ZParcel::storeBlob(ZUID id, ZBinary blob){

}

void ZParcel::storeFile(ZUID id, ZFile file){

}

// /////////////////////////////////////////////////////////////////////////////

bool ZParcel::fetchBool(ZUID id){

}

zu64 ZParcel::fetchUint(ZUID id){

}

zs64 ZParcel::fetchSint(ZUID id){

}

double ZParcel::fetchFloat(ZUID id){

}

ZUID ZParcel::fetchZUID(ZUID id){

}

ZString ZParcel::fetchString(ZUID id){

}

ZBinary ZParcel::fetchBlob(ZUID id){

}

ZFile ZParcel::fetchFile(ZUID id){

}

// /////////////////////////////////////////////////////////////////////////////

ZParcel::fieldid ZParcel::getFieldId(ZString name){
    return 0;
}

ZParcel::fieldtype ZParcel::nameToType(ZString name){
    return nametotype[name];
}

ZString ZParcel::typeName(fieldtype type){
    return typetoname[type];
}

}
