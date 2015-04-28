#include "zparcel4parser.h"
#include "zparcel4page.h"

#include "zlog.h"
#include "zuid.h"
#include "zmap.h"

#define VERSION_4_SIG { 90,80,143,82,128,144,76, 4 } // 0x5A508F5280904C14 // ZPARCEL 1 4
#define SIG_SIZE 8

#define NAME "ZParcel4Parser "

#define DEFAULT_PAGE_SIZE 10 // 2 ^ 10 = 1024 bytes
#define DEFAULT_MAX_PAGES (64 * 1024) // 65536 pages

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

bool ZParcel4Parser::create(){
    if(_init)
        throw ZException(NAME "create: parcel is already open");
    if(!_file->isOpen())
        throw ZException(NAME "create: file is not open");
    ZParcel4Page *header = new HeadPage(this);

    _init = true;
    return true;
}

void ZParcel4Parser::open(){
    if(_init)
        throw ZException(NAME "open: parcel is already open");
    if(!_file->isOpen())
        throw ZException(NAME "open: file is not open");
    ZBinary sig;
    if(_file->read(sig, SIG_SIZE) != SIG_SIZE)
        throw ZException(NAME "open: failed to read signature");
    if(sig != ZBinary(VERSION_4_SIG))
        throw ZException(NAME "open: file is not a version 4 parcel");
    ZBinary ps;
    if(_file->read(ps, 1) != 1)
        throw ZException(NAME "open: failed to read page size");
    zu8 pagepower = ps.tozu8();
    if(pagepower < 5 || pagepower > 32)
        throw ZException(NAME "open: invalid page power");
    _head = new HeadPage(this);
    _head->load(0);

    _init = true;
}

void ZParcel4Parser::setPageSize(zu8 power){
    if(!_init){
        _pagesize = ((zu32)1 << power);
    } else {
        // Page size cannot be changed on existing parcels
        throw ZException(NAME "setPageSize: cannot change page size of existing parcel");
    }
}

void ZParcel4Parser::setMaxPages(zu32 pages){
    _maxpages = pages;
    if(_init)
        writeHeadPage();
}

ZParcel4Parser::fieldid ZParcel4Parser::addField(ZString name, fieldtype type){
    fieldid id = getFieldId(name);
    if(id != NULLFIELD && type == getFieldType(id))
        return id;


    return 0;
}

ZParcel4Parser::fieldid ZParcel4Parser::getFieldId(ZString name){

    return 0;
}

ZParcel4Parser::fieldtype ZParcel4Parser::getFieldType(fieldid id){

    return NULLFIELD;
}

bool ZParcel4Parser::addRecord(FieldList fields){

    return false;
}

void ZParcel4Parser::addBoolRecord(ZParcel4Parser::fieldid field, bool tf){
    ZBinary bin;
    bin.fromzu8((zu8)tf);

}

void ZParcel4Parser::addUintRecord(fieldid field, zu64 num){
    ZBinary bin;
    bin.fromzu64(num);

}

void ZParcel4Parser::addSintRecord(fieldid field, zs64 num){
    ZBinary bin;
    bin.fromzu64((zu64)num);

}

void ZParcel4Parser::addZUIDRecord(fieldid field, ZUID uid){
    ZBinary bin = ZBinary(uid.raw(), 16);

}

void ZParcel4Parser::addFloatRecord(fieldid field, double flt){

}

void ZParcel4Parser::addStringRecord(fieldid field, ZString str){
    ZBinary bin = ZBinary((const zbyte *)str.cc(), str.size());

}

void ZParcel4Parser::addBinaryRecord(fieldid field, ZBinary bin){

}

void ZParcel4Parser::addFileRecord(fieldid field, ZPath file){
    ZString str = file.str();
    ZBinary bin = ZBinary((const zbyte *)str.cc(), str.size());

}

void ZParcel4Parser::readPage(pageid page, ZBinary &data){
    _file->setPos(page * _pagesize);
    data.resize(_pagesize);
    zu64 length = _file->read(data, _pagesize);
    for(zu64 i = 0; i < (length - _pagesize); ++i)
        data[length + i] = 0;
}

ZParcel4Parser::pageid ZParcel4Parser::insertPage(pagetype type){
    switch(type){
        case headpage:
            if(writeHeadPage())
                return 0;
            ELOG("insertPage failed to write head page");
            break;
        case fieldpage:
            zeroPad();

            break;
        case freelistpage:
            break;
        case indexpage:
            break;
        case recordpage:
            break;
        case blobpage:
            break;
        case historypage:
            break;
        case freepage:
            break;
        default:
            ELOG("insertPage unknown page type");
            break;
    }
    return ZU32_MAX;
}

bool ZParcel4Parser::zeroPad(){
    zu64 pad = _file->fileSize() % _pagesize;
    _file->setPos(_file->fileSize());
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
    _file->setPos(page * _pagesize);
    if(_file->write(ZBinary({(zu8)0})) == 1){
        if(addToFreelist(page))
            return true;
    }
    return false;
}

bool ZParcel4Parser::addToFreelist(pageid page){
    if(_freelistpage == 0)
        _freelistpage = insertPage(freelistpage);

    return false;
}

}
