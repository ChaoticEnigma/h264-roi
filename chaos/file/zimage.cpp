/*******************************************************************************
**                                  LibChaos                                  **
**                                 zimage.cpp                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zimage.h"
#include "zlog.h"
#include "zbmp.h"
#include "zppm.h"

#ifdef LIBCHAOS_HAS_PNG
    #include "zpng.h"
#endif
#ifdef LIBCHAOS_HAS_JPEG
    #include "zjpeg.h"
#endif
#ifdef LIBCHAOS_HAS_WEBP
    #include "zwebp.h"
#endif

namespace LibChaos {

const ZMap<ZImage::pixelformat, ZImage::ImageType> ZImage::types = {
    // { type, { channels, depth, planes } }
    { ZImage::RGB24,    { 3,  8, 1 } },
    { ZImage::RGBA32,   { 4,  8, 1 } },
    { ZImage::RGB48,    { 3, 16, 1 } },
    { ZImage::RGBA64,   { 4, 16, 1 } },
    { ZImage::G8,       { 1,  8, 1 } },
    { ZImage::GA16,     { 2,  8, 1 } },
    { ZImage::G16,      { 1, 16, 1 } },
    { ZImage::GA32,     { 2, 16, 1 } },
};

ZImage::ZImage(ZAllocator<zbyte> *alloc) : _alloc(alloc), _width(0), _height(0), _channels(0), _depth(0), _type(UNKNOWN), _buffer(nullptr), _format(NONE), _backend(nullptr){

}

ZImage::ZImage(const ZBinary &image) : ZImage(){
    decodeFormat(image);
}

ZImage::ZImage(zu64 width, zu64 height, ZImage::pixelformat type) : ZImage(){
    setDimensions(width, height, types[type].channels, types[type].depth);
}

ZImage::ZImage(zu64 width, zu64 height, zu8 channels, zu8 depth) : ZImage(){
    setDimensions(width, height, channels, depth);
}

ZImage::ZImage(const zbyte *data, zu64 width, zu64 height, zu8 channels, zu8 depth) : ZImage(){
    setDimensions(width, height, channels, depth);
    copyData(data);
}

ZImage::ZImage(const ZImage &other) : ZImage(other._buffer, other._width, other._height, other._channels, other._depth){

}

ZImage::~ZImage(){
    _alloc->dealloc(_buffer);
    _buffer = nullptr;

    delete _alloc;
}

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
            _buffer = _alloc->alloc(size());
    }
}
void ZImage::zeroData(){
    if(validDimensions()){
        if(!_buffer)
            _buffer = _alloc->alloc(size());
        memset(_buffer, 0, size());
    }
}
void ZImage::copyData(const zbyte *data){
    if(validDimensions()){
        if(!_buffer)
            _buffer = _alloc->alloc(size());
        memcpy(_buffer, data, size());
    }
}
void ZImage::takeData(zbyte *data){
    if(validDimensions()){
        if(_buffer)
            delete[] _buffer;
        // We totally trust the user here. Could go reeeaaalllyyy bad.
        _buffer = data;
    }
}

#define VR_COEFF    1.370705f
#define VG_COEFF    0.698001f
#define UG_COEFF    0.337633f
#define UB_COEFF    1.732446f

void ZImage::convertYUV420toRGB24(zu64 width, zu64 height, const zbyte *ydata, const zbyte *udata, const zbyte *vdata){
    setDimensions(width, height, 3, 8);
    zeroData();

    auto clamp = [](float in){
        return (unsigned char)(in < 0.0f ? 0 : (in > 255.0f ? 255 : in));
    };

    for(zu64 i = 0; i < pixels(); ++i){
        zu64 uvpos = (i / width / 2) * (width / 2) + (i % width / 2);
        pixelAt(i)[0] = clamp((float)ydata[i] + (VR_COEFF * ((float)vdata[uvpos] - 128.0f)));
        pixelAt(i)[1] = clamp((float)ydata[i] - (VG_COEFF * ((float)vdata[uvpos] - 128.0f)) - (UG_COEFF * ((float)udata[uvpos] - 128.0f)));
        pixelAt(i)[2] = clamp((float)ydata[i] + (UB_COEFF * ((float)udata[uvpos] - 128.0f)));
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

void ZImage::setChannels(zu8 channels, const zu8 *expandmask){
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

void ZImage::strip16to8(){
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

ZImage::fileformat ZImage::checkImageFormat(const ZBinary &data){
    if(ZBMP::isBMP(data)){
        return BMP;
    } else if(ZPPM::isPPM(data)){
        return PPM;
#ifdef LIBCHAOS_HAS_PNG
    } else if(ZPNG::isPNG(data)){
        return PNG;
#endif
#ifdef LIBCHAOS_HAS_JPEG
    } else if(ZJPEG::isJPEG(data)){
        return JPEG;
#endif
#ifdef LIBCHAOS_HAS_WEBP
    } else if(ZWebP::isWebP(data)){
        return WEBP;
#endif
    } else {
        return NONE;
    }
}

bool ZImage::isFormatSupported(ZImage::fileformat format){
    switch(format){
    case BMP:   return true;
    case PPM:   return true;
#ifdef LIBCHAOS_HAS_PNG
    case PNG:   return true;
#endif
#ifdef LIBCHAOS_HAS_JPEG
    case JPEG:  return true;
#endif
#ifdef LIBCHAOS_HAS_WEBP
    case WEBP:  return true;
#endif
    default:    return false;
    }
}

bool ZImage::setFormat(fileformat format){
    if(format != _format){
        delete _backend;
        _format = format;

        switch(_format){
        case BMP:
            _backend = new ZBMP(this);
            return true;
        case PPM:
            _backend = new ZPPM(this);
            return true;
#ifdef LIBCHAOS_HAS_PNG
        case PNG:
            _backend = new ZPNG(this);
            return true;
#endif
#ifdef LIBCHAOS_HAS_JPEG
        case JPEG:
            _backend = new ZJPEG(this);
            return true;
#endif
#ifdef LIBCHAOS_HAS_WEBP
        case WEBP:
            _backend = new ZWebP(this);
            return true;
#endif
        case NONE:
            _backend = nullptr;
            return true;
        default:
            _format = NONE;
            _backend = nullptr;
            return false;
        }
    }
    return true;
}

void ZImage::decodeFormat(const ZBinary &data){
    fileformat format = checkImageFormat(data);
    if(format == NONE){
        LOG("Unknown / Invalid Image Format");
    } else {
        setFormat(format);
    }

    ZBinary tmp = data;
    if(_backend){
        bool ok = _backend->decode(&tmp);
        if(!ok){
            destroy();
        }
    }
}

void ZImage::encodeFormat(ZBinary &data){
    if(_backend){
        _backend->encode(&data);
    }
}

}
