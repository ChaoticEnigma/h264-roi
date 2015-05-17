/*******************************************************************************
**                                  LibChaos                                  **
**                                 zimage.cpp                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zimage.h"

namespace LibChaos {

//const ZArray<ZImage::ImageType> ZImage::types = {
//    { ZImage::rgb24,    3,  8   },
//    { ZImage::rgba32,   4,  8   },
//    { ZImage::rgb48,    3,  16  },
//    { ZImage::rgba64,   4,  16  },
//    { ZImage::g8,       1,  8   },
//    { ZImage::ga16,     2,  8   },
//    { ZImage::g16,      1,  16  },
//    { ZImage::ga32,     2,  16  }
//};

const ZMap<ZImage::imagetype, ZImage::ImageType> ZImage::types = {
    { ZImage::rgb24,    { 3,  8, 1 } },
    { ZImage::rgba32,   { 4,  8, 1 } },
    { ZImage::rgb48,    { 3, 16, 1 } },
    { ZImage::rgba64,   { 4, 16, 1 } },
    { ZImage::g8,       { 1,  8, 1 } },
    { ZImage::ga16,     { 2,  8, 1 } },
    { ZImage::g16,      { 1, 16, 1 } },
    { ZImage::ga32,     { 2, 16, 1 } },
};

void ZImage::destroy(){
    _width = 0;
    _height = 0;
    _channels = 0;
    _depth = 0;
    delete[] _buffer;
    _buffer = nullptr;
}

ZImage &ZImage::operator=(const ZImage &other){
    setDimensions(other._width, other._height, other._channels, other._depth);
    copyData(other._buffer);
    return *this;
}

bool ZImage::operator==(const ZImage &other) const {
    return _width == other._width &&
           _height == other._height &&
           _channels == other._channels &&
           _depth == other._depth &&
            // Either both data are null, or are the same
           ((_buffer == nullptr && other._buffer == nullptr) ||
           ((_buffer != nullptr && other._buffer != nullptr) && memcmp(_buffer, other._buffer, size()) == 0));
}

void ZImage::setDimensions(zu64 width, zu64 height, zu8 channels, zu8 depth){
    // Basically, if new dimensions are invalid or result in size() changing, buffer is destroyed
    //if(!width || !height || !channels || !depth || ((zu8)(channels * depth) % (zu8)8) != 0 || (width * height * ((zu8)(channels * depth) / (zu8)8)) != size())
    if(!validDimensions(width, height, channels, depth) || (validDimensions() && (size(width, height, channels, depth) != size())))
        destroy();
    _width = width;
    _height = height;
    _channels = channels;
    _depth = depth;
}

void ZImage::newData(){
    if(validDimensions()){
        if(!_buffer)
            _buffer = new byte[size()];
    }
}
void ZImage::zeroData(){
    if(validDimensions()){
        if(!_buffer)
            _buffer = new byte[size()];
        memset(_buffer, 0, size());
    }
}
void ZImage::copyData(const byte *data){
    if(validDimensions()){
        if(!_buffer)
            _buffer = new byte[size()];
        memcpy(_buffer, data, size());
    }
}
void ZImage::takeData(byte *data){
    if(validDimensions()){
        if(_buffer)
            delete[] _buffer;
        // We totally trust the user here. Could go reeeaaalllyyy bad.
        _buffer = data;
    }
}

void ZImage::convertYUV420toRGB24(zu64 width, zu64 height, const byte *ydata, const byte *udata, const byte *vdata){
    setDimensions(width, height, 3, 8);
    zeroData();

    auto clamp = [](float in){
        return (unsigned char)(in < 0.0f ? 0 : (in > 255.0f ? 255 : in));
    };

    for(zu64 i = 0; i < pixels(); ++i){
        zu64 uvpos = (i / width / 2) * (width / 2) + (i % width / 2);
        pixelAt(i)[0] = clamp((float)ydata[i] + (1.370705f * ((float)vdata[uvpos] - 128.0f)));
        pixelAt(i)[1] = clamp((float)ydata[i] - (0.698001f * ((float)vdata[uvpos] - 128.0f)) - (0.337633f * ((float)udata[uvpos] - 128.0f)));
        pixelAt(i)[2] = clamp((float)ydata[i] + (1.732446f * ((float)udata[uvpos] - 128.0f)));
    }
}

void ZImage::transferImage(ZImage &other){
    setDimensions(other._width, other._height, other._channels, other._depth);
    takeData(other._buffer);
    other._buffer = nullptr;
}

void ZImage::reformat(ZArray<char> before, ZArray<char> after){
    // we'll get to sub-byte channels later
    if(_depth % 8 == 0){

    }
}

void ZImage::setChannels(zu8 channels, const unsigned char *expandmask){
    if(channels != _channels){
        if(isLoaded()){
            ZImage temp(_width, _height, channels, _depth);
            temp.newData();
            if(temp.isLoaded()){
                bool copymask = channels > _channels && expandmask != nullptr;
                for(zu64 i = 0, j = 0; i < temp.realSize() && j < realSize(); i += temp.pixelSize(), j += pixelSize()){
                    if(copymask)
                        memcpy(temp.buffer() + i, expandmask, temp.pixelSize());
                    memcpy(temp.buffer() + i, _buffer + j, MIN(temp.pixelSize(), pixelSize()));
                }
                transferImage(temp);
                return;
            }
        }
        _channels = channels;
    }
}

void ZImage::setDepth(zu8 depth){
    if(depth != _depth){
        if(isLoaded() && _depth % 8 == 0){
            ZImage temp(_width, _height, _channels, depth);
            temp.newData();
            if(temp.isLoaded() && depth % 8 == 0){
                zu8 ibytes = depth / 8;
                zu8 jbytes = _depth / 8;
                bool ismall = ibytes < jbytes;
                zu8 offset = 0;
                if(ismall)
                    offset = jbytes - ibytes;
                else
                    offset = ibytes - jbytes;

                for(zu64 i = 0, j = 0; i < temp.realSize() && j < realSize(); i += ibytes, j += jbytes){
                    if(ismall)
                        memcpy(temp.buffer() + i, _buffer + j + offset, MIN(ibytes, jbytes));
                    else
                        memcpy(temp.buffer() + i + offset, _buffer + j, MIN(ibytes, jbytes));
                }
                transferImage(temp);
                return;
            }
            setDimensions(_width, _height, _channels, depth);
        }
        _depth = depth;
    }
}

void ZImage::setWidth(zu64 width){
    if(width != _width){
        if(isLoaded()){
            ZImage temp(width, _height, _channels, _depth);
            temp.zeroData();
            if(temp.isLoaded()){
                for(zu64 i = 0; i < height(); ++i){
                    memcpy(temp.buffer() + (i * temp.rowSize()), _buffer + (i * rowSize()), MIN(temp.rowSize(), rowSize()));
                }
                transferImage(temp);
                return;
            }
        }
        _width = width;
    }
}

void ZImage::setHeight(zu64 height){
    if(height != _height){
        if(isLoaded()){
            ZImage temp(_width, height, _channels, _depth);
            temp.zeroData();
            if(temp.isLoaded()){
                memcpy(temp.buffer(), _buffer, MIN(temp.realSize(), realSize()));
                transferImage(temp);
                return;
            }
        }
        _height = height;
    }
}

void ZImage::strip16to8bit(){
    if(isLoaded() && _depth == 16){
        ZImage temp(_width, _height, _channels, 8);
        temp.newData();
        if(temp.isLoaded()){
            for(zu64 i  = 0, j = 0; i < realSize() && j < temp.realSize(); i += 2, j += 1){
                zu16 chn = *(zu16*)(_buffer + i);
                chn = chn >> 8;
                temp[j] = (zu8)chn;
            }
            transferImage(temp);
        }
    }
    _depth = 8;
}

}
