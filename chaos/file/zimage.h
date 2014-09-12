#ifndef ZIMAGE_H
#define ZIMAGE_H

#include "zbitmap.h"
#include "zpath.h"

namespace LibChaos {

class ZImage {
public:

    ZImage() : _width(0), _height(0), _channels(0), _depth(0), _buffer(nullptr){}

    ~ZImage(){
        delete[] _buffer;
    }

    zu64 width(){
        return _width;
    }

    zu64 height(){
        return _height;
    }

    zu8 channels(){
        return _channels;
    }

    zu8 depth(){
        return _depth;
    }

    unsigned char *buffer(){
        return _buffer;
    }

public:
    zu64 _width, _height;
    zu8 _channels, _depth;
    unsigned char *_buffer;
};

}

#endif // ZIMAGE_H
