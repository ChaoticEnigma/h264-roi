#ifndef ZIMAGE_H
#define ZIMAGE_H

#include "ztypes.h"
#include <cstring>
//#include "zbitmap.h"
//#include "zpath.h"
//#include "zerror.h"
#include "zarray.h"

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
// takeData() takes ownership of raw data. CAUTION: do not delete memory passed to takeData()

// validDimensions() - check that current dimensions can represent an image
//      true - _buffer may or may not be null
//      false - _buffer must be null

// isLoaded() - check validDimensions() and buffer() is non-null, check this before using raw buffer() access
//      true - dimensions are valid AND buffer() points to image of size()
//      false - dimensions are invalid OR buffer() is null

class ZImage {
public:
    typedef unsigned char byte;

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
    void destroy(){
        _width = 0;
        _height = 0;
        _channels = 0;
        _depth = 0;
        delete[] _buffer;
        _buffer = nullptr;
    }

    ZImage &operator=(const ZImage &other){
        setDimensions(other._width, other._height, other._channels, other._depth);
        copyData(other._buffer);
        return *this;
    }

    bool operator==(const ZImage &other) const {
        return _width == other._width && _height == other._height && _channels == other._channels && _depth == other._depth &&
               ((_buffer == nullptr && other._buffer == nullptr) || (_buffer != nullptr && other._buffer != nullptr)) && memcmp(_buffer, other._buffer, size());
    }

    // Accesses bytes NOT pixels
    byte &operator[](zu64 i){
        return _buffer[i];
    }
    const byte &operator[](zu64 i) const {
        return _buffer[i];
    }

    void setDimensions(zu64 width, zu64 height, zu8 channels, zu8 depth){
        // Basically, if new dimensions are invalid or result in size() changing, buffer is destroyed
        //if(!width || !height || !channels || !depth || ((zu8)(channels * depth) % (zu8)8) != 0 || (width * height * ((zu8)(channels * depth) / (zu8)8)) != size())
        if(!validDimensions(width, height, channels, depth) || (validDimensions() && (size(width, height, channels, depth) != size())))
            destroy();
        _width = width;
        _height = height;
        _channels = channels;
        _depth = depth;
    }

    void newData(){
        if(validDimensions()){
            if(!_buffer)
                _buffer = new byte[size()];
        }
    }
    void zeroData(){
        if(validDimensions()){
            if(!_buffer)
                _buffer = new byte[size()];
            memset(_buffer, 0, size());
        }
    }
    void copyData(const byte *data){
        if(validDimensions()){
            if(!_buffer)
                _buffer = new byte[size()];
            memcpy(_buffer, data, size());
        }
    }
    void takeData(byte *data){
        if(validDimensions()){
            if(_buffer)
                delete[] _buffer;
            // We totally trust the user here. Could go reeeaaalllyyy bad.
            _buffer = data;
        }
    }

    void transferImage(ZImage &other){
        setDimensions(other._width, other._height, other._channels, other._depth);
        takeData(other._buffer);
        other._buffer = nullptr;
    }

    // Need bitwise operations to do this
    // Example: reformat({'R','G','B'}, {'R','G','B','A'});
    // each arbitrary char corresponds to a channel component of a pixel before and after the transform, in the order they are packed in buffer
    // the same char may not be used twice in <before>
    // channels corresponding to a char in <before> and <after> are reordered according to <after> in every pixel
    // channels corresponding to a char in <before> but not <after> are lost in every pixel
    // channels corresponding to a char in <after> but not <before> are zero in every pixel
    // after may be kept in the zimage as a hint about the image content
    void reformat(ZArray<char> before, ZArray<char> after){
        // we'll get to sub-byte channels later
        if(_depth % 8 == 0){

        }
    }

    // if channels is increased, expandmask will be copied to each pixel before original channels are copied
    // expandmask must be at least the new pixel size
    void setChannels(zu8 channels, const unsigned char *expandmask = nullptr){
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

    // For now, can only deal with depths aligned to bytes (multiples of 8)
    // This is dangerous / doesn't work (maybe)
    void setDepth(zu8 depth){
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

    void setWidth(zu64 width){
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

    void setHeight(zu64 height){
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

    bool validDimensions() const {
        return validDimensions(_width, _height, _channels, _depth);
    }

    bool isLoaded() const {
        return validDimensions() && _buffer != nullptr;
    }

    bool isRGB24() const {
        return _channels == 3 && _depth == 8;
    }
    bool isRGB48() const {
        return _channels == 3 && _depth == 16;
    }

    bool isRGBA32() const {
        return _channels == 4 && _depth == 8;
    }
    bool isRGBA64() const {
        return _channels == 4 && _depth == 16;
    }

    zu64 width() const {
        return _width;
    }

    zu64 height() const {
        return _height;
    }

    zu8 channels() const {
        return _channels;
    }

    zu8 depth() const {
        return _depth;
    }

    zu64 pixels() const {
        return _width * _height;
    }
    zu16 pixelSize() const {
        return (_channels * _depth) / 8;
    }
    zu64 rowSize() const {
        return _width * pixelSize();
    }
    zu64 size() const {
        return _width * _height * pixelSize();
    }
    zu64 realSize(){
        return !_buffer ? 0 : size();
    }

    byte *buffer() const {
        return _buffer;
    }

    static inline zu64 pixelSize(zu8 channels, zu8 depth){
        return (zu16)(channels * depth) / (zu16)8;
    }
    static inline zu64 size(zu64 width, zu64 height, zu8 channels, zu8 depth){
        return width * height * pixelSize(channels, depth);
    }
    static inline bool validDimensions(zu64 width, zu64 height, zu8 channels, zu8 depth){
        return width && height && channels && depth && ((zu16)(channels * depth) % (zu16)8) == 0 && size(width, height, channels, depth);
    }

private:
    // Image width / height in pixels
    zu64 _width, _height;
    // Number of channels per pixel
    zu8 _channels;
    // Number of bits per channel
    zu8 _depth;
    // Actual data
    byte *_buffer;
};

}

#endif // ZIMAGE_H
