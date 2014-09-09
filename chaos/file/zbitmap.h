#ifndef ZBITMAP_H
#define ZBITMAP_H

#include "ztypes.h"
#include "zerror.h"
#include "zbinary.h"
#include <cstring>

namespace LibChaos {

struct Pixel24 {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
struct Pixel32 {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};
typedef Pixel24 Pixel;

template <typename pixeltype> class ZBitmapT {
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
        load(data, width, height);
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

    void load(const pixeltype *data, zu64 width, zu64 height){
        clear();
        _width = width;
        _height = height;
        if(pixels()){
            _buffer = new pixeltype[pixels()];
        }
        if(pixels() && _buffer != nullptr && data != nullptr){
            memcpy(_buffer, data, bufferSize());
        }
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
    void width(zu64 width){
        if(width == _width)
            return;
        if(width == 0){
            clear();
            return;
        }
        pixeltype *tmp = _buffer;
        _buffer = new pixeltype[width * _height];
        if(width < _width){
            if(_width && _height && _buffer != nullptr){
                for(zu64 i = 0; i < _height; ++i){
                    void *newrowpos = _buffer + (_width * i * sizeof(pixeltype));
                    void *oldrowpos = tmp + (_width * i * sizeof(pixeltype));
                    memcpy(newrowpos, oldrowpos, _width * sizeof(pixeltype));
                }
            }
        } else if(width > _width){
            for(zu64 i = 0; i < _height; ++i){
                void *newrowpos = _buffer + (_width * i * sizeof(pixeltype));
                void *oldrowpos = tmp + (_width * i * sizeof(pixeltype));
                if(_width && _height && _buffer != nullptr)
                    memcpy(newrowpos, oldrowpos, _width * sizeof(pixeltype));
                memset(newrowpos + (_width * sizeof(pixeltype)), 0, (width - _width) * sizeof(pixeltype));
            }
        }
        delete[] tmp;
        _width = width;
    }
    zu64 height() const {
        return _height;
    }
    void height(zu64 height){
        if(height == _height)
            return;
        if(height == 0){
            clear();
            return;
        }
        pixeltype *tmp = _buffer;
        _buffer = new pixeltype[_width * height];
        if(height < _height){
            if(_width && _height && _buffer != nullptr)
                memcpy(_buffer, tmp, _width * height * sizeof(pixeltype));
        } else if(height > _height){
            if(_width && _height && _buffer != nullptr)
                memcpy(_buffer, tmp, _width * _height * sizeof(pixeltype));
            memset(_buffer + (_width * _height * sizeof(pixeltype)), 0, _width * (height - _height) * sizeof(pixeltype));
        }
        delete[] tmp;
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

typedef ZBitmapT<Pixel> ZBitmap;

}

#endif // ZBITMAP_H
