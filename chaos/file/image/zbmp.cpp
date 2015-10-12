/*******************************************************************************
**                                  LibChaos                                  **
**                                  zbmp.cpp                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zbmp.h"
#include "zfile.h"

#ifdef BI_RGB
    #undef BI_RGB
#endif

#define BI_RGB 0x0000
#define BITMAP_TYPE 0x4D42
#define FILEHEADER_SIZE 14
#define INFOHEADER_SIZE 40

namespace LibChaos {

//! \cond INTERNALS

struct BitmapFileHeader {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
};

struct BitmapInfoHeader {
    unsigned int biSize;
    unsigned int biWidth;
    unsigned int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPelsPerMeter;
    unsigned int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
};

//! \endcond

void readFileHeader(ZReader *bin, BitmapFileHeader *fileh){
    fileh->bfType      = bin->readleu16();
    fileh->bfSize      = bin->readleu32();
    fileh->bfReserved1 = bin->readleu16();
    fileh->bfReserved2 = bin->readleu16();
    fileh->bfOffBits   = bin->readleu32();
}
void readInfoHeader(ZReader *bin, BitmapInfoHeader *infoh){
    infoh->biSize          = bin->readleu32();
    infoh->biWidth         = bin->readleu32();
    infoh->biHeight        = bin->readleu32();
    infoh->biPlanes        = bin->readleu16();
    infoh->biBitCount      = bin->readleu16();
    infoh->biCompression   = bin->readleu32();
    infoh->biSizeImage     = bin->readleu32();
    infoh->biXPelsPerMeter = bin->readleu32();
    infoh->biYPelsPerMeter = bin->readleu32();
    infoh->biClrUsed       = bin->readleu32();
    infoh->biClrImportant  = bin->readleu32();
}

void writeFileHeader(ZWriter *out, const BitmapFileHeader *fileh){
    // 14 byte file header
    out->writeleu16(fileh->bfType);
    out->writeleu32(fileh->bfSize);
    out->writeleu16(fileh->bfReserved1);
    out->writeleu16(fileh->bfReserved2);
    out->writeleu32(fileh->bfOffBits);
}
void writeInfoHeader(ZWriter *out, const BitmapInfoHeader *infoh){
    // 40 byte bitmap info header
    out->writeleu32(infoh->biSize);
    out->writeleu32(infoh->biWidth);
    out->writeleu32(infoh->biHeight);
    out->writeleu16(infoh->biPlanes);
    out->writeleu16(infoh->biBitCount);
    out->writeleu32(infoh->biCompression);
    out->writeleu32(infoh->biSizeImage);
    out->writeleu32(infoh->biXPelsPerMeter);
    out->writeleu32(infoh->biYPelsPerMeter);
    out->writeleu32(infoh->biClrUsed);
    out->writeleu32(infoh->biClrImportant);
}

bool ZBMP::isBMP(const ZBinary &data){
    return (ZBinary::decle16(data.raw()) == BITMAP_TYPE);
}

bool ZBMP::decode(ZReader *input){
    BitmapFileHeader fileh;
    readFileHeader(input, &fileh);

    if(fileh.bfType != BITMAP_TYPE){
        throw ZException("Not a BMP file", BMPError::notabmp, false);
    }
    if(fileh.bfSize - FILEHEADER_SIZE != input->available()){
        throw ZException("Incorrect file size in file header", BMPError::incorrectsize, false);
    }

    BitmapInfoHeader infoh;
    readInfoHeader(input, &infoh);

    if(infoh.biSize != INFOHEADER_SIZE){
        throw ZException("Unsupported info header length", BMPError::badinfoheader, false);
    }
    if(infoh.biCompression != BI_RGB){
        throw ZException(ZString("Unsupported compression: ") << infoh.biCompression, BMPError::badcompression, false);
    }
    if(infoh.biBitCount != 24){
        throw ZException(ZString("Unsupported pixel bit count: ") << infoh.biBitCount, BMPError::badbitcount, false);
    }

    zu64 width = infoh.biWidth;
    zu64 height = infoh.biHeight;

    _image->setDimensions(width, height, bmp_channels, 8);
    ZBinary data(fileh.bfSize - FILEHEADER_SIZE - INFOHEADER_SIZE);
    input->read(data.raw(), data.size());
    _image->takeData(convertBMPDatatoRGB(data.raw(), _image->width(), _image->height()));

    return true;
}

bool ZBMP::encode(ZWriter *output){
    if(!_image->isRGB24()){
        throw ZException(ZString("BMP Write: Invalid channels / depth ") + _image->channels() + " " + _image->depth());
        return false;
    }

    zu64 outsize;
    zbyte *pixeldata = convertRGBtoBMPData(_image->buffer(), _image->width(), _image->height(), outsize);
    if(!pixeldata){
        throw ZException("BMP Write: error in RGB conversion");
        return false;
    }

    // sizes of headers in file
    const zu32 fileheadersize = 14;
    const zu32 infoheadersize = 40;
    const zu32 filesize = fileheadersize + infoheadersize + outsize;

    BitmapFileHeader fileh;
    fileh.bfType = BITMAP_TYPE;
    fileh.bfSize = filesize; // total size of file
    fileh.bfReserved1 = 0;
    fileh.bfReserved2 = 0;
    fileh.bfOffBits = fileheadersize + infoheadersize; // offset to pixel data
    writeFileHeader(output, &fileh);

    BitmapInfoHeader infoh;
    infoh.biSize = infoheadersize; // size of info header
    infoh.biWidth = (unsigned)_image->width();
    infoh.biHeight = (unsigned)_image->height();
    infoh.biPlanes = 1;
    infoh.biBitCount = _image->channels() * _image->depth(); // will always be 24
    infoh.biCompression = 0; // no compression
    infoh.biSizeImage = 0;
    infoh.biXPelsPerMeter = 0;
    infoh.biYPelsPerMeter = 0;
    infoh.biClrUsed = 0;
    infoh.biClrImportant = 0;
    writeInfoHeader(output, &infoh);

    zassert(output->write(pixeldata, outsize) == outsize);
    delete[] pixeldata;

    return true;
}

zbyte *ZBMP::convertBMPDatatoRGB(const zbyte *bmpbuffer, zu32 width, zu32 height){
    if((NULL == bmpbuffer) || (width == 0) || (height == 0))
        return NULL;

    zu32 rowsize = (width * 3) + ((width * 3) % 4);
    zbyte *rgbbuffer = new zbyte[width * height * 3];

    zu64 bmppos = 0;
    zu64 rgbpos = 0;
    for(zu64 y = 0; y < height; ++y){
        for(zu64 x = 0; x < width; ++x){
            bmppos = ((height - y - 1) * rowsize) + (x * 3);
            rgbpos = (y * width * 3) + (x * 3);

            rgbbuffer[rgbpos]     = bmpbuffer[bmppos + 2];
            rgbbuffer[rgbpos + 1] = bmpbuffer[bmppos + 1];
            rgbbuffer[rgbpos + 2] = bmpbuffer[bmppos];
        }
    }
    return rgbbuffer;
}

zbyte *ZBMP::convertRGBtoBMPData(const zbyte *rgbbuffer, zu32 width, zu32 height, zu64 &outsize){
    if((rgbbuffer == NULL) || (width == 0) || (height == 0))
        return NULL;

    zu32 rowsize = (width * 3) + ((width * 3) % 4);
    outsize = height * rowsize;
    zbyte *bmpbuffer = new zbyte[outsize];

    //memset(bmpbuffer, 0, outsize);

    zu64 rgbpos = 0;
    zu64 bmppos = 0;
    for(zu32 y = 0; y < height; ++y){
        for(zu32 x = 0; x < width; ++x){
            rgbpos = (y * width * 3) + (x * 3);                 // position in original buffer
            bmppos = ((height - y - 1) * rowsize) + (x * 3);    // position in padded buffer

            bmpbuffer[bmppos]     = rgbbuffer[rgbpos + 2];      // swap r and b
            bmpbuffer[bmppos + 1] = rgbbuffer[rgbpos + 1];      // g stays
            bmpbuffer[bmppos + 2] = rgbbuffer[rgbpos];          // swap b and r
        }
    }
    return bmpbuffer;

}

}
