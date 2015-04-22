#include "zparcel4parser.h"
#include "zlog.h"
#include "zuid.h"
#include "zmap.h"

#define VERSION_4_SIG { 'P',143,'R',128,144,'L', 1 , 4 }
#define SIG_SIZE 8
#define DEFAULT_PAGE_SIZE 10 // 2 ^ 10 = 1024
#define DEFAULT_MAX_PAGES (64 * 1024)
#define FIELD_NULL 0

#define FREEPAGE        0
#define FIELDPAGE       1
#define FREELISTPAGE    2
#define INDEXPAGE       3
#define RECORDPAGE      4
#define BLOBPAGE        5
#define HISTORYPAGE     6
#define HEADPAGE        80

#define NULLFIELD           0
#define UNSIGNEDINTFIELD    1
#define SIGNEDINTFIELD      2
#define ZUIDFIELD           3
#define STRINGFIELD         4
#define FILEFIELD           5
#define BINARYFIELD         6
#define FLOATFIELD          7

namespace LibChaos {

using namespace ZParcelTypes;
using namespace ZParcelConvert;

static const ZMap<ZString, fieldtype> fieldnametable = {
    { "null",   nullfield },
    { "uint",   unsignedintfield },
    { "sint",   signedintfield },
    { "int",    signedintfield },
    { "zuid",   zuidfield },
    { "uuid",   zuidfield },
    { "guid",   zuidfield },
    { "string", stringfield },
    { "strg",   stringfield },
    { "file",   filefield },
    { "binary", binaryfield },
    { "blob",   binaryfield },
    { "float",  floatfield },
    { "double", floatfield },
};

struct FieldType {
    zu16 id;
    const char *name;
};

static const ZMap<fieldtype, FieldType> fieldmap = {
    { nullfield,        { NULLFIELD,        "null" } },
    { unsignedintfield, { UNSIGNEDINTFIELD, "unsigned int" } },
    { signedintfield,   { SIGNEDINTFIELD,   "signed int" } },
    { zuidfield,        { ZUIDFIELD,        "zuid" } },
    { stringfield,      { STRINGFIELD,      "string" } },
    { filefield,        { FILEFIELD,        "file" } },
    { binaryfield,      { BINARYFIELD,      "binary" } },
    { floatfield,       { FLOATFIELD,       "float"} }
};

static const ZMap<zu16, fieldtype> fieldtypes = {
    { NULLFIELD,        nullfield },
    { UNSIGNEDINTFIELD, unsignedintfield },
    { SIGNEDINTFIELD,   signedintfield },
    { ZUIDFIELD,        zuidfield },
    { STRINGFIELD,      stringfield },
    { FILEFIELD,        filefield },
    { BINARYFIELD,      binaryfield },
    { FLOATFIELD,       floatfield },
};

ZParcel4Parser::ZParcel4Parser(ZFile *file) : _file(file), _init(false){
    // Default options
    setPageSize(DEFAULT_PAGE_SIZE);
    _maxpages = DEFAULT_MAX_PAGES;
    _freelistpage = 0;
    _fieldpage = 0;
    _indexpage = 0;
    _recordpage = 0;
}

bool ZParcel4Parser::create(){
    if(_init)
        ELOG("create: overwriting existing parcel");
    _init = false;
    if(!writeHeadPage()){
        LOG(_pagepower << " " << _maxpages);
        ELOG("create: failed to write head page");
        return false;
    }
    _init = true;
    return true;
}

bool ZParcel4Parser::open(){
    if(_init)
        ELOG("open: parcel already initialized");
    _init = false;
    if(!loadHeadPage()){
        ELOG("open: failed to load head page");
        return false;
    }
    _init = true;
    return true;
}

void ZParcel4Parser::setPageSize(zu8 power){
    // Cannot be changed on existing parcels
    if(!_init){
        _pagepower = power;
        _pagesize = ((zu32)1 << _pagepower);
    } else {
        ELOG("cannot change page size of existing parcel");
    }
}

void ZParcel4Parser::setMaxPages(zu32 pages){
    _maxpages = pages;
    if(_init)
        writeHeadPage();
}

fieldid ZParcel4Parser::addField(ZString name, fieldtype type){
    fieldid id = getFieldId(name);
    if(id != FIELD_NULL && type == getFieldType(id))
        return id;


    return 0;
}

fieldid ZParcel4Parser::getFieldId(ZString name){

    return 0;
}

fieldtype ZParcel4Parser::getFieldType(fieldid id){

    return nullfield;
}

bool ZParcel4Parser::addRecord(FieldList fields){

    return false;
}

fieldtype ZParcel4Parser::fieldTypeNameToFieldType(ZString name){
    return fieldnametable[name];
}
ZString ZParcel4Parser::getFieldTypeName(fieldtype type){
    return fieldmap[type].name;
}

zu16 ZParcel4Parser::getFieldFileId(fieldtype type){
    return fieldmap[type].id;
}
fieldtype ZParcel4Parser::fieldFileIdToFieldType(zu16 type){
    return fieldtypes[type];
}

bool ZParcel4Parser::readPage(pageid page, ZBinary &data){
    _file->setPos(page * _pagesize);
    return (_file->read(data, _pagesize) == _pagesize);
}

bool ZParcel4Parser::loadHeadPage(){
    _file->setPos(0);
    ZBinary sig;
    if(_file->read(sig, SIG_SIZE) != SIG_SIZE)
        return false;
    if(sig != ZBinary(VERSION_4_SIG))
        return false;

    ZBinary nums;
    if(_file->read(nums, 1) != 1)
        return false;
    _pagepower = ZParcelConvert::fromFile8Bits(nums);
    setPageSize(_pagepower);

    nums.clear();
    if(_file->read(nums, 4) != 4)
        return false;
    _maxpages = fromFile32Bits(nums);

    nums.clear();
    if(_file->read(nums, 4) != 4)
        return false;
    _freelistpage = fromFile32Bits(nums);

    nums.clear();
    if(_file->read(nums, 4) != 4)
        return false;
    _fieldpage = fromFile32Bits(nums);

    nums.clear();
    if(_file->read(nums, 4) != 4)
        return false;
    _indexpage = fromFile32Bits(nums);

    nums.clear();
    if(_file->read(nums, 4) != 4)
        return false;
    _recordpage = fromFile32Bits(nums);

    return true;
}

pageid ZParcel4Parser::insertPage(pagetype type){
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

bool ZParcel4Parser::writeHeadPage(){
    _file->setPos(0);
    ZBinary sig = VERSION_4_SIG;
    if(_file->write(sig) != SIG_SIZE)
        return false;
    if(_file->write(toFile8Bits(_pagepower)) != sizeof(zu8))
        return false;
    if(_file->write(toFile32Bits(_maxpages)) != sizeof(zu32))
        return false;
    if(_file->write(toFile32Bits(_freelistpage)) != sizeof(zu32))
        return false;
    if(_file->write(toFile32Bits(_fieldpage)) != sizeof(zu32))
        return false;
    if(_file->write(toFile32Bits(_indexpage)) != sizeof(zu32))
        return false;
    if(_file->write(toFile32Bits(_recordpage)) != sizeof(zu32))
        return false;

    return true;
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

// /////////////////////////////////////////
// ParcelPage
// /////////////////////////////////////////

ZParcel4Parser::ParcelPage::ParcelPage(ZFile *file, zu32 page, zu32 pagesize) : _file(file), _page(page), _pagesize(pagesize), _rwpos(0){

}

zu64 ZParcel4Parser::ParcelPage::read(zbyte *dest, zu64 size){
    _file->setPos(_page * _pagesize + _rwpos);
    return _file->read(dest, size);
}

zu64 ZParcel4Parser::ParcelPage::write(const zbyte *src, zu64 size){
    return _file->write(src, size);
}

bool ZParcel4Parser::ParcelPage::atEnd() const{
    return _file->atEnd();
}

// /////////////////////////////////////////
// FieldPage
// /////////////////////////////////////////

ZParcel4Parser::FieldPage::FieldPage(ZFile *file, zu32 page, zu32 pagesize) : ParcelPage(file, page, pagesize){
    ZBinary buff;
    buff.resize(1);
    read(buff.raw(), 1);
    if(fromFile8Bits(buff) != FIELDPAGE){
        ELOG("FieldPage created on wrong page type");
        return;
    }
    buff.resize(4);
    read(buff.raw(), 4);
    _prevpage = fromFile32Bits(buff);
    setPos(_pagesize - 4);
    read(buff.raw(), 4);
    _nextpage = fromFile32Bits(buff);

}

}
