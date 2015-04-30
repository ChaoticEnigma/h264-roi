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
    if(sig.readzu64() != VERSION_4_MASK)
        throw ZException(NAME "open: invalid file signature");
    zu8 pagepower = sig.readzu8();
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
    fieldid id = getFieldId(name);
    if(id != 0)
        throw ZException(NAME "addField: field already exists");

    return getFieldId(name);
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
    _file->setPos(page * _pagesize);
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
        _file->setPos(i * _pagesize);
        length = _file->read(buffer, _pagesize);
        if(length != _pagesize)
            throw ZException(NAME "insertPage: read unaligned page from file");
        buffer.rewind();
        if(buffer.readzu8() == FREEPAGE){
            buffer.rewind();
            buffer.writezu8(RESERVEDPAGE);
            _file->setPos(i * _pagesize);
            _file->write(buffer);
            return i;
        }
    }
    // Write a new page at the end of the file
    buffer.resize(_pagesize);
    buffer.rewind();
    buffer.writezu8(RESERVEDPAGE);
    _file->setPos(_pagecount * _pagesize);
    _file->write(buffer);
    return _pagecount++;
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

    return false;
}

}
