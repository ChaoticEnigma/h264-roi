/*******************************************************************************
**                                  LibChaos                                  **
**                                  zimage.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZIMAGE_H
#define ZIMAGE_H

#include "ztypes.h"
#include <cstring>
//#include "zbitmap.h"
//#include "zpath.h"
//#include "zerror.h"
#include "zarray.h"
#include "zmap.h"

namespace LibChaos {

// Notes on Usage:
// Guarantee: buffer will never be allocated to a nonzero size different than size(). buffer may be null when size() is nonzero.
// size() returns the size of the image in bytes, but this does NOT guarantee the buffer() is allocated

// setDimensions() sets the virtual size of the image, may delete buffer, but does not allocate buffer
//      check the actual dimensions after setting dimensions
//      partial dimensions are allowed
//      no image may be loaded unless all dimenions are valid
//      (channels * depth) % 8 must be zero. dimensions are invalid otherwise
// copyData() copies raw data of size() into buffer, buffer is allocated if necessary
// takeData() takes ownership of raw data. CAUTION: do not free memory passed to takeData()

// validDimensions() - check that current dimensions can represent an image
//      true - _buffer may or may not be null
//      false - _buffer must be null

// isLoaded() - check validDimensions() and buffer() is non-null, check this before using raw buffer() access
//      true - dimensions are valid AND buffer() points to image of size()
//      false - dimensions are invalid OR buffer() is null

class ZImage {
public:
    typedef unsigned char byte;

    enum imagetype {
        unknown = 0,
        rgb24   = 1,
        rgba32  = 2,
        rgb48   = 3,
        rgba64  = 4,
        g8      = 5,
        ga16    = 6,
        g16     = 7,
        ga32    = 8,
    };

//    struct ImageType {
//        imagetype type;
//        zu8 channels;
//        zu8 depth;
//    };
//    static const ZArray<ImageType> types;

    struct ImageType {
        zu8 channels;
        zu8 depth;
    };
    static const ZMap<imagetype, ImageType> types;


public:
    ZImage() : _width(0), _height(0), _channels(0), _depth(0), _buffer(nullptr){

    }
    ZImage(zu64 width, zu64 height, zu8 channels = 3, zu8 depth = 8) : ZImage(){
        setDimensions(width, height, channels, depth);
    }
    ZImage(const byte *data, zu64 width, zu64 height, zu8 channels = 3, zu8 depth = 8) : ZImage(){
        setDimensions(width, height, channels, depth);
        copyData(data);
    }
    ZImage(const ZImage &other) : ZImage(other._buffer, other._width, other._height, other._channels, other._depth){

    }

    ~ZImage(){
        delete[] _buffer;
        _buffer = nullptr;
    }

    void destroy();

    ZImage &operator=(const ZImage &other);

    bool operator==(const ZImage &other) const;

    // Accesses bytes NOT pixels
    inline byte &operator[](zu64 i){
        return _buffer[i];
    }
    inline const byte &operator[](zu64 i) const {
        return _buffer[i];
    }

    inline byte *pixelAt(zu64 i) const {
        return &_buffer[i * pixelSize()];
    }
    inline byte *pixelAt(zu64 x, zu64 y) const {
        return &_buffer[(y * _width + x) * pixelSize()];
    }

    void setDimensions(zu64 width, zu64 height, zu8 channels, zu8 depth);

    void newData();
    void zeroData();
    void copyData(const byte *data);
    void takeData(byte *data);

    // Takes Y, U, and V planes separately
    // Planes are expected unpadded and packed width * height or width * height / 4
    void convertYUV420toRGB24(zu64 width, zu64 height, const byte *ydata, const byte *udata, const byte *vdata);

    void transferImage(ZImage &other);

    // Need bitwise operations to do this
    // Example: reformat({'R','G','B'}, {'R','G','B','A'});
    // each arbitrary char corresponds to a channel component of a pixel before and after the transform, in the order they are packed in buffer
    // the same char may not be used twice in <before>
    // channels corresponding to a char in <before> and <after> are reordered according to <after> in every pixel
    // channels corresponding to a char in <before> but not <after> are lost in every pixel
    // channels corresponding to a char in <after> but not <before> are zero in every pixel
    // after may be kept in the zimage as a hint about the image content
    void reformat(ZArray<char> before, ZArray<char> after);

    // if channels is increased, expandmask will be copied to each pixel before original channels are copied
    // expandmask must be at least the new pixel size
    void setChannels(zu8 channels, const unsigned char *expandmask = nullptr);

    // For now, can only deal with depths aligned to bytes (multiples of 8)
    // This is dangerous / doesn't work (maybe)
    void setDepth(zu8 depth);

    void setWidth(zu64 width);
    void setHeight(zu64 height);

    void strip16to8bit();

    inline bool validDimensions() const {
        return validDimensions(_width, _height, _channels, _depth);
    }
    static inline bool validDimensions(zu64 width, zu64 height, zu8 channels, zu8 depth){
        return width && height && channels && depth && ((zu16)(channels * depth) % (zu16)8) == 0 && size(width, height, channels, depth);
    }

    inline bool isLoaded() const {
        return validDimensions() && _buffer != nullptr;
    }

    inline bool isRGB24() const {
        return _channels == 3 && _depth == 8;
    }
    inline bool isRGB48() const {
        return _channels == 3 && _depth == 16;
    }

    inline bool isRGBA32() const {
        return _channels == 4 && _depth == 8;
    }
    inline bool isRGBA64() const {
        return _channels == 4 && _depth == 16;
    }

    inline zu64 width() const {
        return _width;
    }

    inline zu64 height() const {
        return _height;
    }

    inline zu8 channels() const {
        return _channels;
    }

    inline zu8 depth() const {
        return _depth;
    }

    inline zu64 pixels() const {
        return _width * _height;
    }

    inline zu16 pixelSize() const {
        return (_channels * _depth) / 8;
    }
    static inline zu64 pixelSize(zu8 channels, zu8 depth){
        return (zu16)(channels * depth) / (zu16)8;
    }

    inline zu64 rowSize() const {
        return _width * pixelSize();
    }

    inline zu64 size() const {
        return _width * _height * pixelSize();
    }
    static inline zu64 size(zu64 width, zu64 height, zu8 channels, zu8 depth){
        return width * height * pixelSize(channels, depth);
    }

    inline zu64 realSize(){
        return !_buffer ? 0 : size();
    }

    inline byte *buffer() const {
        return _buffer;
    }

private:
    // Image width / height in pixels
    zu64 _width, _height;
    // Number of planes
    zu8 _planes;
    // Number of channels per pixel
    zu8 _channels;
    // Number of bits per channel
    zu8 _depth;
    // Image type
    imagetype _type;
    // Actual data
    unsigned char *_buffer;
};

}

#endif // ZIMAGE_H
