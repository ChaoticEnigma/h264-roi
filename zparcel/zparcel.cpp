#include "zparcel.h"

#include <iterator>
#include <algorithm>
#include <cstring>
#include <assert.h>

#include "zfile.h"
#include "zlog.h"
#include "ztypes.h"
#include "zerror.h"

#define ZPARCEL_SIG_SIZE 8
#define ZPARCEL_VERSION_2_SIG { 'P','A','R','C','E','L','0','2' }
#define ZPARCEL_VERSION_3_SIG { 'P',143,'R',128,144,'L', 0 , 3  }
#define ZPARCEL_VERSION_4_SIG { 'P',143,'R',128,144,'L', 0 , 4  }

#define BUFFER_SIZE 32768 // (1 << 15)

namespace LibChaos {

ZParcel::ZParcel() : _headsize(0){}

ZParcel::~ZParcel(){
    close();
}

bool ZParcel::create(ZPath path){
    if(!_parcel.open(path, ZFile::modewrite)){
        ELOG("Failed to open parcel file");
        return false;
    }
    _path = path;

    // Write parcel signature
    if(!writeSig()){
        ELOG("Failed to write parcel signature");
        return false;
    }

    currpos = 0;
    return true;
}

bool ZParcel::open(ZPath file){
    _path = file;
    if(!_parcel.open(_path, ZFile::moderead)){
        ELOG("Could not open parcel");
        return false;
    }

    _type = readSig(_path);
    switch(_type){
        case version3: {
            zu64 headersize;
            if(_parcel.read((zbyte*)&headersize, sizeof(zu64)) != sizeof(zu64)) // 8
                return false;
            ZBinary header;
            if(_parcel.read(header, headersize) != headersize) // headersize
                return false;
            header.rewind();

            // Read until read position at end
            char cstr[256];
            while(!header.atEnd()){
                unsigned char keylen;
                ParcelSection sec;

                if(header.read(&keylen, sizeof(unsigned char)) != sizeof(unsigned char)) // 1
                    return false;
                if(header.read((zbyte*)cstr, keylen) != keylen) // keylen
                    return false;
                if(header.read((zbyte*)&sec.pos, sizeof(zu64)) != sizeof(zu64)) // 8
                    return false;
                if(header.read((zbyte*)&sec.len, sizeof(zu64)) != sizeof(zu64)) // 8
                    return false;

                cstr[keylen] = '\0';
                _index.push(cstr, sec);
            }
            _headsize = ZPARCEL_SIG_SIZE + sizeof(zu64) + headersize;
            break;
        }

        default:
            _type = versionunknown;
            ELOG("Unknown parcel version");
            return false;
    }

    return true;
}

bool ZParcel::testParcel(ZPath file){
    return (readSig(file) != versionunknown);
}

void ZParcel::close(){
    _parcel.close();
}

bool ZParcel::addFile(ZString name, ZPath path){
    zu64 total = ZFile::fileSize(path);
    // Add size/offset of file to table
    _index.push(name, { currpos, total, path, false });
    currpos += total;
    return true;
}

bool ZParcel::addData(ZString name, const ZBinary &data){
    if(!data.size()){
        ELOG("Data is empty");
        return false;
    }

    ZPath tmppath = name + ".tmp";
    ZFile tmpfile;
    if(!tmpfile.open(tmppath, ZFile::modewrite))
        return false;
    if(!tmpfile.write(data))
        return false;
    tmpfile.close();
    bool ret = addFile(name, tmppath);
    if(ret)
        _index.last().istmp = true;
    return ret;
}

bool ZParcel::getSection(ParcelSection locat, ZBinary &out){
    switch(_type){
        case version2:
        case version3: {
            zu64 newpos = _parcel.setPos(_headsize + locat.pos);
            if(newpos != (_headsize + locat.pos))
                return false;
            if(_parcel.read(out, locat.len) != locat.len)
                return false;
            return true;
            break;
        }

        default:
            break;
    }
    return false;
}

ZParcelSection ZParcel::readSection(ZString name){
    return ZParcelSection(&_parcel, _headsize + _index[name].pos, _index[name].len);
}

ZString ZParcel::keyByIndex(zu64 inx){
    return _index.key(inx);
}

bool ZParcel::getByName(ZString key, ZBinary &out){
    return getSection(_index[key], out);
}
ZBinary ZParcel::operator[](ZString name){
    ZBinary out;
    getByName(name, out);
    return out;
}
ZBinary ZParcel::getByIndex(zu64 index){
    ZBinary out;
    getSection(_index[index], out);
    return out;
}

/*
zu64 ZParcel::finishParcel(){
    if(!_parcel.isOpen()){
        ELOG("Parcel file is not open");
        return 0;
    }

    // Make header
    ZBinary header;
    for(zu64 i = 0; i < _index.size(); ++i){
        unsigned char keylen = MIN(_index.key(i).size(), 255);
        if(header.write(&keylen, sizeof(unsigned char)) != sizeof(unsigned char)) // 1
            return false;
        if(header.write((const zbyte*)_index.key(i).cc(), keylen) != keylen) // keylen
            return false;
        if(header.write((const zbyte*)&_index.val(i).pos, sizeof(zu64)) != sizeof(zu64)) // 8
            return false;
        if(header.write((const zbyte*)&_index.val(i).len, sizeof(zu64)) != sizeof(zu64)) // 8
            return false;
    }

    // Write header
    zu64 hsize = header.size();
    _parcel.setPos(ZPARCEL_SIG_SIZE);
    _parcel.write((zbyte*)&hsize, sizeof(zu64));
    _parcel.write(header);

    // Calculate total size and resize parcel file
    zu64 filetotal = ZPARCEL_SIG_SIZE + sizeof(zu64) + hsize;
    for(zu64 i = 0; i < _index.size(); ++i)
        filetotal += _index[i].len;
    _parcel.resizeFile(filetotal);

    // Copy content files into parcel
    unsigned char *buffer = new unsigned char[BUFFER_SIZE];
    zu64 total = _parcel.getPos();
    for(zu64 i = 0; i < _index.size(); ++i){
        ZFile infile;
        if(!infile.open(_index[i].file)){
            ELOG("Could not open \"" << _index[i].file << "\"");
            return 0;
        }

        LOG("Copying \"" << _index[i].file << "\" " << _index[i].len);

        // Copy file to parcel
        zu64 fltotal = 0;
        zu64 readsize;
        do {
            readsize = infile.read(buffer, BUFFER_SIZE);
            if(!readsize)
                break;
            if(_parcel.write(buffer, readsize) != readsize)
                return 0;
            fltotal += readsize;
        } while(readsize == BUFFER_SIZE);

        if(fltotal != _index[i].len){
            ELOG("Copied file lengths do not match " << fltotal << " " << _index[i].len);
            return 0;
        }

        total += fltotal;

        infile.close();
        if(_index[i].istmp)
            infile.remove();
    }
    delete[] buffer;

    _parcel.close();

    if(total != filetotal)
        ELOG("Parcel totals do not match " << total << " " << filetotal);

    return total;
}
*/

zu64 ZParcel::unParcel(){
    ZPath base = _path;
    base.last().replace(".", "_").append("_cont");
    base.getAbsolute();

    unsigned char *buffer = new unsigned char[BUFFER_SIZE];
    zu64 total = 0;
    for(zu64 i = 0; i < _index.size(); ++i){

        zu64 len = _index[i].len;
        ZPath outpath = base + _index.key(i);
        ZFile::createDirsTo(outpath);
        ZParcelSection section(&_parcel, _index[i].pos, _index[i].len);
        ZFile outfl;
        if(!outfl.open(outpath, ZFile::modewrite)){
            ELOG("Out file " << outpath << " not opened");
            continue;
        }

        zu64 fltotal = 0;
        zu64 readsize;
        do {
            readsize = section.read(buffer, BUFFER_SIZE);
            if(!readsize)
                break;
            zu64 writesize = outfl.write(buffer, readsize);
            if(writesize != readsize)
                break;
            fltotal += writesize;
        } while(readsize == BUFFER_SIZE);

        if(fltotal != len){
            ELOG("File " << outpath << " not fully copied " << fltotal << " " << len);
            continue;
        }

        total += fltotal;
    }
    delete[] buffer;
    return total;
}

zu64 ZParcel::parcelContSize(){
    zu64 total = 0;
    for(zu64 i = 0; i < _index.size(); ++i){
        total += _index[i].len;
    }
    return total;
}

ZPath ZParcel::tmpParcelPath(){
    ZPath tmp = _path;
    tmp.last() << ".tmp";
    return tmp;
}

ZParcel::parceltype ZParcel::readSig(ZPath path){
    ZFile file;
    if(!file.open(path)){
        ELOG("Failed to open parcel file");
        return versionunknown;
    }

    // Read 8-byte parcel file signature
    ZBinary sig;
    if(file.read(sig, ZPARCEL_SIG_SIZE) != ZPARCEL_SIG_SIZE)
        return versionunknown;

    if(sig == ZBinary(ZPARCEL_VERSION_2_SIG)){
        return version2;
    } else if(sig == ZBinary(ZPARCEL_VERSION_3_SIG)){
        return version3;
    } else if(sig == ZBinary(ZPARCEL_VERSION_4_SIG)){
        return version4;
    } else {
        return versionunknown;
    }
}

bool ZParcel::writeSig(){
    if(!_parcel.isOpen()){
        ELOG("Parcel is not open");
        return false;
    }
    // Write 8-byte parcel file signature
    ZBinary sig = ZPARCEL_VERSION_3_SIG;
    return (_parcel.write(sig) == ZPARCEL_SIG_SIZE);
}

}
