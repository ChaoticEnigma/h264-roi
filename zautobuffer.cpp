#include "zautobuffer.h"
#include <cstring>

namespace LibChaos {

char *copy(const char *src, zu64 len){
    char *dest = new char[len];
    memcpy(dest, src, len);
    return dest;
}

ZAutoBuffer::ZAutoBuffer(){

}
ZAutoBuffer::~ZAutoBuffer(){
    for(zu64 i = 0; i < refs.size(); ++i){
        if(refs[i].type == type_ptr){
            delete[] refs[i].ptr;
        }
    }
}

ZAutoBuffer &ZAutoBuffer::add(ZString str){
    // Copy ZString content to new memory += str.size() bytes
    refs.push({ type_ptr, copy(str.cc(), str.size()), 0, str.size(), 0 });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(ZString str, zu64 pos, zu64 len){
    // Copy sub-ZString content to new memory += len bytes
    ZString tmp = str.substr(pos, len);
    refs.push({ type_ptr, copy(tmp.cc(), tmp.size()), 0, tmp.size(), 0 });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(char *ptr, zu64 len){
    // Copy character string to new memory (no offset) += len bytes
    refs.push({ type_ptr, copy(ptr, len), 0, len, 0 });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(char *ptr, zu64 pos, zu64 len){
    refs.push({ type_ptr, copy(ptr + pos, len), 0, len, 0 });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(const char *ptr, zu64 len){
    // Pointer to existing memory (no offset) += 0 bytes
    refs.push({ type_const_ptr, (char *)ptr, 0, len, 0 });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(const char *ptr, zu64 pos, zu64 len){
    // Pointer to existing memory += 0 bytes
    refs.push({ type_const_ptr, (char*)(ptr + pos), 0, len, 0 });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(ZFile file){
    refs.push({ type_file, copy(file.path().str().cc(), file.path().str().size()), file.path().str().size(), file.flsize(), 0 });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(ZFile file, zu64 pos, zu64 len){
    refs.push({ type_sub_file, copy(file.path().str().cc(), file.path().str().size()), file.path().str().size(), len, pos });
    return *this;
}
ZAutoBuffer &ZAutoBuffer::add(zu64 num){
    // Copy integer to new memory += 0 bytes
    refs.push({ type_integer, NULL, 0, ZString::ItoS(num).size(), num });
    return *this;
}

ZString readFile(){
    //int num = (chars[0] << 24) + (chars[1] << 16) + (chars[2] << 8) + chars[3];
    return ZString();
}

}
