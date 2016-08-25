#include "zparcel.h"
#include "zmap.h"
#include "zlog.h"

#define ZPARCEL_SIG "ZPARCEL"
#define ZPARCEL_SIG_LEN 7

#define ZPARCEL_HEAD_NODE 16

namespace LibChaos {

static const ZMap<ZParcel::objtype, ZString> typetoname = {
    { ZParcel::NULLOBJ,   "null" },
    { ZParcel::UINTOBJ,   "uint" },
    { ZParcel::SINTOBJ,   "sint" },
    { ZParcel::FLOATOBJ,  "float" },
    { ZParcel::ZUIDOBJ,   "zuid" },
    { ZParcel::STRINGOBJ, "string" },
    { ZParcel::FILEOBJ,   "file" },
    { ZParcel::BLOBOBJ,   "binary" },
};

struct BTreeNode {
    ZUID uid;
    zu64 lnode;
    zu64 rnode;
    zu64 data;
    zu64 size;
};

struct FreeNode {
    zu64 next;
    zu64 pos[4];
    zu64 len[4];
};

ZParcel::ZParcel() : _version(UNKNOWN){

}

ZParcel::~ZParcel(){
    close();
}

void ZParcel::create(ZPath path){
    _version = VERSION1;
    _file.open(path, ZFile::READWRITE | ZFile::TRUNCATE);
    _file.write(ZPARCEL_SIG, ZPARCEL_SIG_LEN);
    _file.writeu8(_version);
    // Freelist location
    _file.writebeu64(0);
}

bool ZParcel::open(ZPath file){
    _file.open(file, ZFile::READWRITE);
    ZBinary sig(ZPARCEL_SIG_LEN);
    _file.read(sig.raw(), sig.size());
    if(sig != ZBinary(ZPARCEL_SIG, ZPARCEL_SIG_LEN))
        return false;
    _version = _file.readu8();
    _freelist = _file.readbeu64();
    return true;
}

void ZParcel::close(){
    _file.close();
}

// /////////////////////////////////////////////////////////////////////////////

void ZParcel::storeNull(ZUID id){
    _storeObject(id, NULLOBJ, ZBinary());
}

void ZParcel::storeBool(ZUID id, bool bl){
    ZBinary data(bl ? 1 : 0);
    _storeObject(id, BOOLOBJ, data);
}

void ZParcel::storeUint(ZUID id, zu64 num){
    _storeObject(id, UINTOBJ, ZBinary().writebeu64(num));
}

void ZParcel::storeSint(ZUID id, zs64 num){
    _storeObject(id, SINTOBJ, ZBinary().writebes64(num));
}

void ZParcel::storeFloat(ZUID id, double num){
    _storeObject(id, FLOATOBJ, ZBinary().writedouble(num));
}

void ZParcel::storeZUID(ZUID id, ZUID uid){
    _storeObject(id, ZUIDOBJ, uid.bin());
}

void ZParcel::storeString(ZUID id, ZString str){
    _storeObject(id, STRINGOBJ, ZBinary(str.bytes(), str.size()));
}

void ZParcel::storeBlob(ZUID id, ZBinary blob){
    _storeObject(id, BLOBOBJ, blob);
}

void ZParcel::storeFile(ZUID id, ZFile file){
    ZString name = file.path().relativeTo(ZPath(_file.path()).parent()).str();
    _storeObject(id, FILEOBJ, ZBinary(name.bytes(), name.size()));
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

ZParcel::objtype ZParcel::getType(ZUID id){

}

ZString ZParcel::typeName(objtype type){
    return typetoname[type];
}

// /////////////////////////////////////////////////////////////////////////////

void ZParcel::_storeObject(ZUID id, ZParcel::objtype type, ZBinary &data){

}

ZReader *ZParcel::_getReader(ZUID id){
    return nullptr;
}

}
