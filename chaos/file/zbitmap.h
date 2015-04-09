/*******************************************************************************
**                                  LibChaos                                  **
**                                  zbitmap.h                                 **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZBITMAP_H
#define ZBITMAP_H

#include "ztypes.h"
#include "zexception.h"
#include "zbinary.h"
#include <cstring>

namespace LibChaos {

struct Pixel8 {
    unsigned char g;
};
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
typedef Pixel8 PixelG;
typedef Pixel24 PixelRGB;
typedef Pixel32 PixelRGBA;

typedef PixelRGB Pixel;

static_assert(sizeof(Pixel8) == 1, "Pixel8 has incorrect size");
static_assert(sizeof(Pixel24) == 3, "Pixel24 has incorrect size");
static_assert(sizeof(Pixel32) == 4, "Pixel32 has incorrect size");

template <typename pixeltype> class ZBitmapT {
public:
    ZBitmapT() : _width(0), _height(0), _buffer(nullptr){

    }
    ZBitmapT(zu64 width, zu64 height) : _width(width), _height(height), _buffer(nullptr){
        if(pixels()){
            _buffer = new pixeltype[pixels()];
        }
    }
    ZBitmapT(const pixeltype *data, zu64 width, zu64 height) : ZBitmapT(width, height){
        load(data, width, height);
    }
    ZBitmapT(const ZBitmapT<pixeltype> &other) : ZBitmapT(other._buffer, other._width, other._height){

    }

    ~ZBitmapT(){
        clear();
    }

    ZBitmapT<pixeltype> &operator=(const ZBitmapT<pixeltype> &other){
        load(other._buffer, other._width, other._height);
        return *this;
    }

    void load(const pixeltype *data, zu64 width, zu64 height){
        if(width * height){
            if(bufferSize() != width * height * pixelSize())
                clear();
            _width = width;
            _height = height;
            if(!_buffer){
                _buffer = new pixeltype[pixels()];
            }
            if(_buffer && data)
                memcpy(_buffer, data, bufferSize());
            else
                clear();
        } else {
            clear();
        }
    }

    template <typename newtype>
    ZBitmapT<newtype> recast(int fill = 0) const {
        newtype *buff = new newtype[pixels()];
        if(sizeof(newtype) > pixelSize())
            memset(buff, fill, pixels() * sizeof(newtype));

        for(zu64 i = 0; i < pixels(); ++i){
            memcpy((unsigned char *)buff + (i * sizeof(newtype)), (unsigned char *)_buffer + (i * pixelSize()), MIN(sizeof(newtype), pixelSize()));
            //buff[i] = _buffer[i];
        }

        ZBitmapT<newtype> out(buff, _width, _height);
        delete[] buff;
        return out;
    }

    void zero(){
        if(_buffer != nullptr){
            memset(_buffer, 0, bufferSize());
        }
    }

    pixeltype &operator[](zu64 i){
        return _buffer[i];
    }
    const pixeltype &operator[](zu64 i) const {
        return _buffer[i];
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
        // No change
        if(width == _width)
            return;
        // Width set to zero, preserve height
        if(width == 0){
            zu64 height = _height;
            clear();
            _height = height;
            return;
        }
        // Height is zero, save width
        if(_height == 0){
            _width = width;
            return;
        }
        pixeltype *tmp = new pixeltype[width * _height];
        if(tmp){
            if(width < _width){
                // If new buffer smaller
                if(_buffer){
                    for(zu64 i = 0; i < _height; ++i){
                        unsigned char *oldrowpos = (unsigned char *)_buffer + (_width * i * pixelSize());
                        unsigned char *newrowpos = (unsigned char *)tmp + (width * i * pixelSize());

                        // Copy old data
                        memcpy(newrowpos, oldrowpos, width * pixelSize());
                    }
                }

            } else if(width > _width){
                // If new buffer larger
                for(zu64 i = 0; i < _height; ++i){
                    unsigned char *oldrowpos = (unsigned char *)_buffer + (_width * i * pixelSize());
                    unsigned char *newrowpos = (unsigned char *)tmp + (width * i * pixelSize());

                    // Copy old data (if any)
                    if(_width && _height && _buffer)
                        memcpy(newrowpos, oldrowpos, _width * pixelSize());

                    // Zero new memory
                    memset(newrowpos + (_width * pixelSize()), 0, (width - _width) * pixelSize());
                }
            }
        }
        delete[] _buffer;
        _buffer = tmp;
        _width = width;
    }

    zu64 height() const {
        return _height;
    }
    void height(zu64 height){
        // No change
        if(height == _height)
            return;
        // Height set to zero, preserve width
        if(height == 0){
            zu64 width = _width;
            clear();
            _width = width;
            return;
        }
        // Width is zero, save height
        if(_width == 0){
            _height = height;
            return;
        }
        pixeltype *tmp = new pixeltype[_width * height];
        if(height < _height){
            // If new buffer smaller

            // Copy old data
            if(_width && _height && _buffer && tmp)
                memcpy(tmp, _buffer, _width * height * pixelSize());
        } else if(height > _height){
            // If new buffer larger

            // Copy old data (if any)
            if(_width && _height && _buffer && tmp)
                memcpy(tmp, _buffer, bufferSize());

            // Zero new memory
            memset((unsigned char *)tmp + (bufferSize()), 0, _width * (height - _height) * pixelSize());
        }
        delete[] _buffer;
        _buffer = tmp;
        _height = height;
    }

    zu64 pixels() const {
//        if(_buffer == nullptr)
//            return 0;
        return _width * _height;
    }
    zu64 pixelSize() const {
        return sizeof(pixeltype);
    }
    zu64 bufferSize() const {
        return pixels() * pixelSize();
    }

    pixeltype *buffer() const {
        return _buffer;
    }

private:
    zu64 _width;
    zu64 _height;
    pixeltype *_buffer;
};

typedef ZBitmapT<PixelRGB> ZBitmapRGB;
typedef ZBitmapT<PixelRGBA> ZBitmapRGBA;

typedef ZBitmapT<Pixel> ZBitmap;

}

#endif // ZBITMAP_H
