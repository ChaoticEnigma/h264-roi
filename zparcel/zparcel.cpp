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

struct ParcelHeader {
    zbyte sig[ZPARCEL_SIG_LEN];
    zu8 version   : 8;
    zu64 treehead : 64;
    zu64 freehead : 64;
};

struct ParcelTreeNode {
    ZUID uid; // 16 bytes
    zu64 lnode;
    zu64 rnode;
    zu8 type;
};

struct ParcelFreeNode {
    zu64 pos[4];
    zu64 len[4];
    zu64 next;
};

// /////////////////////////////////////////////////////////////////////////////

ZParcel::ZParcel() : _version(UNKNOWN){
    LOG(sizeof(ParcelHeader));
    LOG(sizeof(ParcelTreeNode));
    LOG(sizeof(ParcelFreeNode));
}

ZParcel::~ZParcel(){
    close();
}

void ZParcel::create(ZPath path){
    _version = VERSION1;
    _file.open(path, ZFile::READWRITE | ZFile::TRUNCATE);
    _file.write((const zbyte *)ZPARCEL_SIG, ZPARCEL_SIG_LEN);
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
    _version = (parceltype)_file.readu8();
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
    ZBinary data;
    data.writebeu64(num);
    _storeObject(id, UINTOBJ, data);
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

    return false;
}

zu64 ZParcel::fetchUint(ZUID id){

    return 0;
}

zs64 ZParcel::fetchSint(ZUID id){

    return 0;
}

double ZParcel::fetchFloat(ZUID id){

    return 0;
}

ZUID ZParcel::fetchZUID(ZUID id){

    return ZUID();
}

ZString ZParcel::fetchString(ZUID id){

    return ZString();
}

ZBinary ZParcel::fetchBlob(ZUID id){

    return ZBinary();
}

ZFile ZParcel::fetchFile(ZUID id){

    return ZFile();
}

// /////////////////////////////////////////////////////////////////////////////

ZParcel::objtype ZParcel::getType(ZUID id){

    return NULLOBJ;
}

ZString ZParcel::typeName(objtype type){
    return typetoname[type];
}

// /////////////////////////////////////////////////////////////////////////////

void ZParcel::_storeObject(ZUID id, ZParcel::objtype type, const ZBinary &data){

}

ZReader *ZParcel::_getReader(ZUID id){
    return nullptr;
}

}
