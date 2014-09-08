#ifndef ZBITMAP_H
#define ZBITMAP_H

#include "ztypes.h"
#include "zerror.h"
#include <cstring>

namespace LibChaos {

template <typename pixeltype> class ZBitmapT {
public:
    struct Pixel24 {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
    typedef Pixel24 Pixel;

    struct Pixel32 {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

public:
    ZBitmapT() : _width(0), _height(0), _buffer(nullptr){

    }
    ZBitmapT(zu64 width, zu64 height, bool zero = true) : _width(width), _height(height), _buffer(nullptr){
        if(pixels()){
            _buffer = new pixeltype[pixels()];
            if(zero && _buffer != nullptr){
                memset(_buffer, 0, bufferSize());
            }
        }
    }
    ZBitmapT(const pixeltype *data, zu64 width, zu64 height) : ZBitmapT(width, height, false){
        if(pixels() && _buffer != nullptr && data != nullptr){
            memcpy(_buffer, data, bufferSize());
        }
    }
    ZBitmapT(const ZBitmapT<pixeltype> &other) : ZBitmapT(other.buffer(), other.width(), other.height()){

    }

    ~ZBitmapT(){
        clear();
    }

    ZBitmapT<pixeltype> &operator=(const ZBitmapT<pixeltype> &other){
        if(other.pixels() && other.buffer() != nullptr){
            if(pixels() != other.pixels()){
                clear();
                _width = other.width();
                _height = other.height();
            }
            if(_buffer == nullptr){
                _buffer = new pixeltype[pixels()];
            }
            memcpy(_buffer, other.buffer(), bufferSize());
        } else {
            clear();
        }
        return *this;
    }

    void set(zu64 i, pixeltype p){
        if(pixels() > i)
            _buffer[i] = p;
    }

    void clear(){
        _width = 0;
        _height = 0;
        delete[] _buffer;
        _buffer = nullptr;
    }

    zu64 width() const {
        return _width;
    }
    void width(const zu64 &width){
        throw ZError("Not Implmented");
        _width = width;
    }
    zu64 height() const {
        return _height;
    }
    void height(const zu64 &height){
        throw ZError("Not Implmented");
        _height = height;
    }

    zu64 pixels() const {
//        if(_buffer == nullptr)
//            return 0;
        return _width * _height;
    }
    zu64 bufferSize() const {
        return pixels() * sizeof(pixeltype);
    }

    pixeltype *buffer() const {
        return _buffer;
    }

private:
    zu64 _width;
    zu64 _height;
    pixeltype *_buffer;
};

typedef ZBitmapT<ZBitmapT::Pixel> ZBitmap;

}

#endif // ZBITMAP_H
