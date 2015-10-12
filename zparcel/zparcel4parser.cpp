#include "zparcel4parser.h"
#include "zparcel4page.h"

#include "zlog.h"
#include "zuid.h"
#include "zmap.h"

#define NAME "ZParcel4Parser "

#define HEADPAGEID 0
#define NULLFIELD ZPARCEL_4_NULL

namespace LibChaos {

struct FieldType {
    zu16 id;
    const char *name;
};

//static const ZMap<zu16, fieldtype> fieldtypes = {
//    { ZPARCEL_4_NULL,        nullfield },
//    { ZPARCEL_4_UNSIGNEDINT, unsignedintfield },
//    { ZPARCEL_4_SIGNEDINT,   signedintfield },
//    { ZPARCEL_4_ZUID,        zuidfield },
//    { ZPARCEL_4_STRING,      stringfield },
//    { ZPARCEL_4_FILE,        filefield },
//    { ZPARCEL_4_BINARY,      binaryfield },
//    { ZPARCEL_4_FLOAT,       floatfield },
//};

ZParcel4Parser::ZParcel4Parser(ZFile *file) : _file(file), _init(false){
    // Default options
    setPageSize(DEFAULT_PAGE_SIZE);
    _maxpages = DEFAULT_MAX_PAGES;
}

void ZParcel4Parser::create(){
    if(_init)
        throw ZException(NAME "create: parcel is already open");
    if(!_file->isOpen())
        throw ZException(NAME "create: file is not open");
    _head = new HeadPage(this);
    _head->init();
    _head->save();

    _init = true;
}

void ZParcel4Parser::open(){
    if(_init)
        throw ZException(NAME "open: parcel is already open");
    if(!_file->isOpen())
        throw ZException(NAME "open: file is not open");
    ZBinary sig;
    if(_file->read(sig, 9) != 9)
        throw ZException(NAME "open: failed to read signature");
    if(sig.readbeu64() != VERSION_4_MASK)
        throw ZException(NAME "open: invalid file signature");
    zu8 pagepower = sig.readu8();
    if(pagepower < 5 || pagepower > 32)
        throw ZException(NAME "open: invalid page power");
    _pagesize = (zu32)1 << pagepower;
    _head = new HeadPage(this);
    _head->load(HEADPAGEID);

    _init = true;
}

void ZParcel4Parser::setPageSize(zu8 power){
    // Page size cannot be changed on existing parcels
    if(_init)
        throw ZException(NAME "setPageSize: cannot change page size of existing parcel");
    _pagesize = ((zu32)1 << power);
}

void ZParcel4Parser::setMaxPages(zu32 pages){
    _maxpages = pages;
    if(_init){
        ((HeadPage*)_head)->_maxpages = _maxpages;
        _head->save();
    }
}

ZParcel4Parser::fieldid ZParcel4Parser::addField(ZString name, fieldtype type){
    fieldid id = getField(name);
    if(id != 0)
        throw ZException(NAME "addField: field already exists");

    return getField(name);
}

ZParcel4Parser::fieldid ZParcel4Parser::getField(ZString name){

    return 0;
}

ZParcel4Parser::fieldtype ZParcel4Parser::getFieldType(fieldid id){

    return NULLFIELD;
}

ZUID ZParcel4Parser::addRecord(Record record){
    return addRecords({ record }).front();
}

ZList<ZUID> ZParcel4Parser::addRecords(ZList<Record> records){
    if(!_init)
        throw ZException(NAME "addRecords: parcel is not open");

    ZList<ZUID> uids;
    for(zu64 i = 0; i < records.size(); ++i){
        ZUID uid = records[i].uid;
        if(uid == ZUID_NIL)
            uid = ZUID(ZUID::TIME);
        ZList<Field> fields = records[i].fields;
        for(zu64 j = 0; j < fields.size(); ++j){
            fieldid id = fields[j].id;
            ZBinary data = fields[j].data;
            fieldtype type = getFieldType(id);
            switch(type){
            case ZPARCEL_4_BOOL:
                //                addBoolData(id, (data == "true" ? true : false));
                break;
            case ZPARCEL_4_UINT:
                //                addUintData(id, data.tozu64());
                break;
            case ZPARCEL_4_SINT:
                //                addSintData(id, (zs64)data.tozu64());
                break;
            case ZPARCEL_4_FLOAT:
                //                addFloatData(id, data.toFloat());
                break;
            case ZPARCEL_4_ZUID:
                //                addZUIDData(id, ZUID(data));
                break;
            case ZPARCEL_4_STRING:
                //                addStringData(id, data);
                break;
            case ZPARCEL_4_BINARY:
                //                addBinaryData(id, ZBinary((const zbyte *)data.cc(), data.size());
                break;
            case ZPARCEL_4_FILE:
                //                addFileData(id, ZPath(data));
                break;
            default:
                throw ZException(NAME "addRecords: Invalid type");
            }
        }
        uids.push(uid);
    }
    return uids;
}

ZBinary ZParcel4Parser::formatBool(bool tf){
    return ZBinary::fromzu8((zu8)tf);
}

ZBinary ZParcel4Parser::formatUint(zu64 uint){
    return ZBinary::fromzu64(uint);
}

ZBinary ZParcel4Parser::formatSint(zs64 sint){
    return ZBinary::fromzu64((zu64)sint);
}

ZBinary ZParcel4Parser::formatFloat(double flt){
    ZBinary bin(4);
    return bin;
}

ZBinary ZParcel4Parser::formatZUID(ZUID uid){
    return ZBinary(uid.raw(), 16);
}

ZBinary ZParcel4Parser::formatString(ZString str){
    return ZBinary(str.cc(), str.size());
}

ZBinary ZParcel4Parser::formatBinary(ZBinary bin){
    return bin;
}

ZBinary ZParcel4Parser::formatFile(ZPath file){
    ZString str = file.str();
    return ZBinary(str.cc(), str.size());
}

void ZParcel4Parser::readPage(pageid page, ZBinary &data){
    _file->seek(page * _pagesize);
    data.resize(_pagesize);
    zu64 length = _file->read(data, _pagesize);
    if(!length)
        throw ZException(NAME "readPage: failed to read page");
    for(zu64 i = length; i < _pagesize; ++i)
        data[i] = 0;
}

void ZParcel4Parser::writePage(ZParcel4Parser::pageid page, const ZBinary &data){
    ZBinary tmp(data);
    zu64 length = tmp.size();
    tmp.resize(_pagesize);
    for(zu64 i = length; i < _pagesize; ++i)
        tmp[i] = 0;
    _file->seek(page * _pagesize);
    length = _file->write(tmp);
    if(length != _pagesize)
        throw ZException(NAME "writePage: failed to write page");
}

ZParcel4Parser::pageid ZParcel4Parser::insertPage(){
    ZBinary buffer;
    zu64 length;
    // Find a free page
    // TODO: Fix this sick filth
    for(zu64 i = 1; i < _pagecount; ++i){
        _file->seek(i * _pagesize);
        length = _file->read(buffer, _pagesize);
        if(length != _pagesize)
            throw ZException(NAME "insertPage: read unaligned page from file");
        buffer.rewind();
        if(buffer.readu8() == FREEPAGE){
            buffer.rewind();
            buffer.writeu8(RESERVEDPAGE);
            _file->seek(i * _pagesize);
            _file->write(buffer);
            return i;
        }
    }
    // Write a new page at the end of the file
    buffer.resize(_pagesize);
    buffer.rewind();
    buffer.writeu8(RESERVEDPAGE);
    _file->seek(_pagecount * _pagesize);
    _file->write(buffer);
    return _pagecount++;
}

bool ZParcel4Parser::zeroPad(){
    zu64 pad = _file->fileSize() % _pagesize;
    _file->seek(_file->fileSize());
    if(pad){
        ZBinary zero;
        zero.fill(0, 1024);
        zu32 fill = MIN(pad, 1024);
        _file->write(zero.raw(), fill);
        pad -= fill;
    }
    return true;
}

bool ZParcel4Parser::freePage(pageid page){
    _file->seek(page * _pagesize);
    if(_file->write(ZBinary::fromzu8(0)) != 1)
        throw ZException(NAME "freePage: failed to change page type");
    if(addToFreelist(page))
        return true;
    return false;
}

bool ZParcel4Parser::addToFreelist(pageid page){

    return false;
}

}
