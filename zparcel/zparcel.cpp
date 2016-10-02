#include "zparcel.h"
#include "zmap.h"
#include "zlog.h"
#include "zerror.h"

#define ZPARCEL_SIG "ZPARCEL"
#define ZPARCEL_SIG_LEN 7

#define ZPARCEL_HEADER_LEN (ZPARCEL_SIG_LEN + 1 + 8 + 8 + 8)

#define ZPARCEL_NODE_LEN (16 + 8 + 8 + 1 + 4)
#define ZPARCEL_FREE_NODE_COUNT 4

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
    zu64 tailptr  : 64;

    bool read(ZFile &file){
        file.read(sig, ZPARCEL_SIG_LEN);
        if(::memcmp(sig, ZPARCEL_SIG, ZPARCEL_SIG_LEN) != 0)
            return false;
        version = file.readu8();
        treehead = file.readbeu64();
        freehead = file.readbeu64();
        tailptr = file.readbeu64();
        return true;
    }

    bool write(ZFile &file){
        file.write(sig, ZPARCEL_SIG_LEN);
        file.writeu8(version);
        file.writebeu64(treehead);
        file.writebeu64(freehead);
        file.writebeu64(tailptr);
        return true;
    }
};

struct ParcelTreeNode {
    ZUID uid; // 16 bytes
    zu64 lnode : 64;
    zu64 rnode : 64;
    zu8 type   : 8;
    zu32 crc   : 32;

    bool read(ZFile &file){
        zbyte id[16];
        file.read(id, 16);
        uid.fromRaw(id);
        lnode = file.readbeu64();
        rnode = file.readbeu64();
        type = file.readu8();
        crc = file.readbeu32();
        return true;
    }

    bool write(ZFile &file){
        file.write(uid.raw(), 16);
        file.writebeu64(lnode);
        file.writebeu64(rnode);
        file.writeu8(type);
        file.writebeu32(crc);
        return true;
    }
};

struct ParcelFreeNode {
    zu64 pos[ZPARCEL_FREE_NODE_COUNT];
    zu64 len[ZPARCEL_FREE_NODE_COUNT];
    zu64 next;

    bool read(ZFile &file){
        for(int i = 0 ; i < ZPARCEL_FREE_NODE_COUNT; ++i)
            pos[i] = file.readbeu64();
        for(int i = 0 ; i < ZPARCEL_FREE_NODE_COUNT; ++i)
            len[i] = file.readbeu64();
        next = file.readbeu64();
        return true;
    }

    bool write(ZFile &file){
        for(int i = 0 ; i < ZPARCEL_FREE_NODE_COUNT; ++i)
            if(!file.writebeu64(pos[i]))
                return false;
        for(int i = 0 ; i < ZPARCEL_FREE_NODE_COUNT; ++i)
            if(!file.writebeu64(len[i]))
                return false;
        if(!file.writebeu64(next))
            return false;
        return true;
    }
};

// /////////////////////////////////////////////////////////////////////////////

ZParcel::ZParcel() : _version(UNKNOWN){

}

ZParcel::~ZParcel(){
    close();
}

ZParcel::parcelerror ZParcel::create(ZPath file){
    _version = VERSION1;
    _file.open(file, ZFile::READWRITE | ZFile::TRUNCATE);
    if(!_file.isOpen())
        return ERR_OPEN;

    _treehead = ZU64_MAX;
    _freelist = ZU64_MAX;
    _tail = ZPARCEL_HEADER_LEN;

    if(!_writeHeader(0))
        return ERR_WRITE;
    return OK;
}

ZParcel::parcelerror ZParcel::open(ZPath file){
    _file.open(file, ZFile::READWRITE);
    if(!_file.isOpen())
        return ERR_OPEN;

    ParcelHeader header;
    _file.seek(0);
    if(!header.read(_file))
        return ERR_READ;
    if(::memcmp(header.sig, ZPARCEL_SIG, ZPARCEL_SIG_LEN) != 0)
        return ERR_SIG;
    if(header.version >= MAX_PARCELTYPE)
        return ERR_VERSION;

    _version = (parceltype)header.version;
    _treehead = header.treehead;
    _freelist = header.freehead;
    _tail = header.tailptr;

    return OK;
}

void ZParcel::close(){
    _file.close();
}

// /////////////////////////////////////////////////////////////////////////////

ZParcel::parcelerror ZParcel::storeNull(ZUID id){
    _storeObject(id, NULLOBJ, ZBinary());
    return OK;
}

ZParcel::parcelerror ZParcel::storeBool(ZUID id, bool bl){
    ZBinary data(1);
    data[0] = (bl ? 1 : 0);
    _storeObject(id, BOOLOBJ, data);
    return OK;
}

ZParcel::parcelerror ZParcel::storeUint(ZUID id, zu64 num){
    ZBinary data;
    data.writebeu64(num);
    _storeObject(id, UINTOBJ, data);
    return OK;
}

ZParcel::parcelerror ZParcel::storeSint(ZUID id, zs64 num){
    ZBinary data;
    data.writebes64(num);
    _storeObject(id, SINTOBJ, data);
    return OK;
}

ZParcel::parcelerror ZParcel::storeFloat(ZUID id, double num){
    ZBinary data;
    data.writedouble(num);
    _storeObject(id, FLOATOBJ, data);
    return OK;
}

ZParcel::parcelerror ZParcel::storeZUID(ZUID id, ZUID uid){
    _storeObject(id, ZUIDOBJ, uid.bin());
    return OK;
}

ZParcel::parcelerror ZParcel::storeString(ZUID id, ZString str){
    _storeObject(id, STRINGOBJ, ZBinary(str.bytes(), str.size()));
    return OK;
}

ZParcel::parcelerror ZParcel::storeBlob(ZUID id, ZBinary blob){
    _storeObject(id, BLOBOBJ, blob);
    return OK;
}

ZParcel::parcelerror ZParcel::storeFile(ZUID id, ZPath path){
    // Get relative path
    ZString name = ZPath(path).relativeTo(ZPath(_file.path()).parent()).str();

    // Open file
    ZFile file(path, ZFile::READ);
    if(!file.isOpen())
        return ERR_READ;
    // Get filesize
    zu64 filesize = file.fileSize();

    // Add node with filename
    ZBinary bin;
    bin.writebeu16(name.size());
    bin.write(name.bytes(), name.size());
    _storeObject(id, FILEOBJ, bin, filesize);

    // Get offset
    ObjectInfo info;
    _getObjectInfo(id, info);

    // Write payload
    zu64 poff = info.offset + 8 + 2 + name.size();
    if(_file.seek(poff) == poff)
        return ERR_SEEK;
    ZBinary buff;
    while(!file.atEnd()){
        buff.clear();
        if(file.read(buff, 1 << 15))
            return ERR_READ;
        if(_file.write(buff))
            return ERR_WRITE;
    }
    return OK;
}

// /////////////////////////////////////////////////////////////////////////////

bool ZParcel::fetchBool(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != BOOLOBJ)
        throw ZException("fetchBool called for wrong Object type");
    _file.seek(info.offset);
    return !!_file.readu8();
}

zu64 ZParcel::fetchUint(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != UINTOBJ)
        throw ZException("fetchUint called for wrong Object type");
    _file.seek(info.offset);
    return _file.readbeu64();
}

zs64 ZParcel::fetchSint(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != SINTOBJ)
        throw ZException("fetchSint called for wrong Object type");
    _file.seek(info.offset);
    return _file.readbes64();
}

double ZParcel::fetchFloat(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != FLOATOBJ)
        throw ZException("fetchFloat called for wrong Object type");
    _file.seek(info.offset);
    return _file.readdouble();
}

ZUID ZParcel::fetchZUID(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != ZUIDOBJ)
        throw ZException("fetchZUID called for wrong Object type");
    _file.seek(info.offset);
    zbyte bytes[16];
    _file.read(bytes, 16);
    return ZUID(ZUID::UNINIT).fromRaw(bytes);
}

ZString ZParcel::fetchString(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != STRINGOBJ)
        throw ZException("fetchString called for wrong Object type");
    _file.seek(info.offset);
    zu64 len = _file.readbeu64();
    ZBinary bin;
    _file.read(bin, len);
    return ZString(bin.raw(), len);
}

ZBinary ZParcel::fetchBlob(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != ZUIDOBJ)
        throw ZException("fetchBlob called for wrong Object type");
    _file.seek(info.offset);
    zu64 len = _file.readbeu64();
    ZBinary bin;
    _file.read(bin, len);
    return bin;
}

ZString ZParcel::fetchFile(ZUID id, zu64 *offset, zu64 *size){
    ObjectInfo info;
    _getObjectInfo(id, info);
    if(info.type != FILEOBJ)
        throw ZException("fetchFile called for wrong Object type");
    _file.seek(info.offset);

    zu64 total = _file.readbeu64();
    zu16 strlen = _file.readbeu16();
    zu64 flsize = total - strlen - 2;

    ZBinary strbin;
    _file.read(strbin, strlen);
    ZString name(strbin.raw(), strlen);

//    ZFile file(name, ZFile::WRITE);

    // Dump payload
//    ZBinary buff;
//    zu64 len = 0;
//    _file.seek(info.offset + 8 + 2 + strlen);
//    while(len < size){
//        buff.clear();
//        size = size - _file.read(buff, MIN(1 << 15, size));
//        ZASSERT(file.write(buff), "write failed");
//    }

    if(offset != nullptr)
        *offset = info.offset + 8 + 2 + strlen;
    if(size != nullptr)
        *size = flsize;

    return name;
}

// /////////////////////////////////////////////////////////////////////////////

ZParcel::objtype ZParcel::getType(ZUID id){
    ObjectInfo info;
    _getObjectInfo(id, info);
    return info.type;
}

ZString ZParcel::typeName(objtype type){
    return typetoname[type];
}

// /////////////////////////////////////////////////////////////////////////////

zu64 ZParcel::_objectSize(ZParcel::objtype type, zu64 size){
    switch(type){
        case NULLOBJ:
            return ZPARCEL_NODE_LEN;
        case BOOLOBJ:
            return ZPARCEL_NODE_LEN + 1;
        case UINTOBJ:
        case SINTOBJ:
            return ZPARCEL_NODE_LEN + 4;
        case FLOATOBJ:
            return ZPARCEL_NODE_LEN + 10;
        case ZUIDOBJ:
            return ZPARCEL_NODE_LEN + 16;
        case STRINGOBJ:
        case BLOBOBJ:
        case FILEOBJ:
            // User defined types must have a 64-bit length at the beginning of the payload.
        default:
            return ZPARCEL_NODE_LEN + 8 + size;
    }
}

ZParcel::parcelerror ZParcel::_storeObject(ZUID id, objtype type, const ZBinary &data, zu64 trailsize){
    ParcelTreeNode newnode;
    newnode.uid = id;
    newnode.type = type;
    newnode.lnode = ZU64_MAX;
    newnode.rnode = ZU64_MAX;

    // First find somewhere to add the node

    // Total needed size
    zu64 size = _objectSize(type, data.size() + trailsize);
    // File offset, fallback write to tail
    zu64 offset = _tail;
    // Selected freelist entry
    ParcelFreeNode fnode;
    // Selected freelist entry index
    int fi;

    // Search the free list
    zu64 fnext = _freelist;
    while(true){
        if(fnext == ZU64_MAX)
            break;
        _file.seek(fnext);
        fnode.read(_file);
        for(int i = 0; i < ZPARCEL_FREE_NODE_COUNT; ++i){
            if(fnode.len[i] >= size){
                // Got first free entry large enough
                offset = fnode.pos[i];
                fi = i;
                break;
            }
        }
    }

    // Find where to insert into tree
    zu64 next = _treehead;
    if(next == ZU64_MAX){
        // Tree head is uninitialzied
        _treehead = offset;
        // Rewrite header with new tree head
        if(_writeHeader(0))
            return ERR_WRITE;
    } else {
        // Search tree
        ParcelTreeNode node;
        while(true){
            if(_file.seek(next) != next){
                // Cannot seek next offset
                return ERR_TREE;
            }
            // Read the node
            node.read(_file);
            // Compare
            int cmp = node.uid.compare(id);
            if(cmp < 0){
                if(node.rnode == ZU64_MAX){
                    node.rnode = offset;
                    break;
                }
                next = node.rnode;
            } else if(cmp > 0){
                if(node.lnode == ZU64_MAX){
                    node.lnode = offset;
                    break;
                }
                next = node.lnode;
            } else {
                // Object already exists
                return ERR_EXISTS;
            }
        }

        // Rewrite the previous node
        node.crc = 0;
//        if(_file.seek(next) != next)
//            return ERR_TREE;
        if(!node.write(_file))
            return ERR_WRITE;
    }

    // Update freelist entry
    fnode.len[fi] -= size;
    if(_file.seek(fnext) != fnext)
        return ERR_SEEK;
    if(!fnode.write(_file))
        return ERR_WRITE;

    // Write the node
    newnode.crc = 0;
    if(_file.seek(offset) != offset)
        return ERR_SEEK;
    if(!newnode.write(_file))
        return ERR_WRITE;
    // Write a length if needed (string, blob, file, or custom)
    if(type >= STRINGOBJ)
        _file.writebeu64(data.size() + trailsize);
    // Write the payload
    if(!_file.write(data))
        return ERR_WRITE;

    // Update tail
    if(offset + size > _tail)
        _tail = offset + size;
    if(!_writeHeader(0))
        return ERR_WRITE;

    return OK;
}

void ZParcel::_getObjectInfo(ZUID id, ObjectInfo &info){
    // Search the tree
    zu64 next = _treehead;
    ParcelTreeNode node;
    while(true){
        if(next == ZU64_MAX){
            // Hit bottom of tree
            info.error = ERR_NOEXIST;
            break;
        }
        if(_file.seek(next) != next){
            // Cannot seek next offset
            info.error = ERR_TREE;
            break;
        }

        node.read(_file);

        int cmp = node.uid.compare(id);
        if(cmp < 0){
            next = node.rnode;
        } else if(cmp > 0){
            next = node.lnode;
        } else {
            // Get the object info
            // TODO: Check Object length and CRC
            info.error = OK;
            info.type = node.type;
            info.offset = next + ZPARCEL_NODE_LEN;
            info.length = _objectSize(info.type, _file.readbeu64());
            break;
        }
    }
}

bool ZParcel::_writeHeader(zu64 offset){
    ParcelHeader header;
    ::memcpy(header.sig, ZPARCEL_SIG, ZPARCEL_SIG_LEN);
    header.version = _version;
    header.treehead = _treehead;
    header.freehead = _freelist;
    header.tailptr = _tail;

    ZASSERT(_file.seek(offset) == offset, "Seek failed");
    if(!header.write(_file))
        return false;
    return true;
}

}
