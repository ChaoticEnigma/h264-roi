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

namespace LibChaos {

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

void readFileHeader(ZBinary &bin, BitmapFileHeader *fileh){
    fileh->bfType      = bin.readle16();
    fileh->bfSize      = bin.readle32();
    fileh->bfReserved1 = bin.readle16();
    fileh->bfReserved2 = bin.readle16();
    fileh->bfOffBits   = bin.readle32();
}
void readInfoHeader(ZBinary &bin, BitmapInfoHeader *infoh){
    infoh->biSize          = bin.readle32();
    infoh->biWidth         = bin.readle32();
    infoh->biHeight        = bin.readle32();
    infoh->biPlanes        = bin.readle16();
    infoh->biBitCount      = bin.readle16();
    infoh->biCompression   = bin.readle32();
    infoh->biSizeImage     = bin.readle32();
    infoh->biXPelsPerMeter = bin.readle32();
    infoh->biYPelsPerMeter = bin.readle32();
    infoh->biClrUsed       = bin.readle32();
    infoh->biClrImportant  = bin.readle32();
}

ZBinary writeFileHeader(const BitmapFileHeader *fileh){
    // 14 byte file header
    ZBinary out;
    out.writele16(fileh->bfType);
    out.writele32(fileh->bfSize);
    out.writele16(fileh->bfReserved1);
    out.writele16(fileh->bfReserved2);
    out.writele32(fileh->bfOffBits);
    return out;
}
ZBinary writeInfoHeader(const BitmapInfoHeader *infoh){
    // 40 byte bitmap info header
    ZBinary out;
    out.writele32(infoh->biSize);
    out.writele32(infoh->biWidth);
    out.writele32(infoh->biHeight);
    out.writele16(infoh->biPlanes);
    out.writele16(infoh->biBitCount);
    out.writele32(infoh->biCompression);
    out.writele32(infoh->biSizeImage);
    out.writele32(infoh->biXPelsPerMeter);
    out.writele32(infoh->biYPelsPerMeter);
    out.writele32(infoh->biClrUsed);
    out.writele32(infoh->biClrImportant);
    return out;
}

bool ZBMP::decode(ZBinary &data_in, YImageBackend::ReadOptions *options){
    BitmapFileHeader fileh;
    readFileHeader(data_in, &fileh);

    if(fileh.bfType != BITMAP_TYPE){
        throw ZException("Not a BMP file", BMPError::notabmp, false);
    }
    if(fileh.bfSize != data_in.size()){
        throw ZException("Incorrect file size in file header", BMPError::incorrectsize, false);
    }

    BitmapInfoHeader infoh;
    data_in.setPos(14);
    readInfoHeader(data_in, &infoh);

    if(infoh.biSize != 40){
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
    unsigned char *pixels = convertBMPDatatoRGB(data_in.raw() + fileh.bfOffBits, _image->width(), _image->height());
    _image->takeData(pixels);

    return true;
}

bool ZBMP::encode(ZBinary &data_out, YImageBackend::WriteOptions *options){
    if(!_image->isRGB24()){
        throw ZException(ZString("BMP Write: Invalid channels / depth ") + _image->channels() + " " + _image->depth());
        return false;
    }

    zu64 outsize;
    zbyte *pixdata = convertRGBtoBMPData(_image->buffer(), _image->width(), _image->height(), outsize);
    if(!pixdata){
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
    zassert(data_out.write(writeFileHeader(&fileh)) == fileheadersize);

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
    zassert(data_out.write(writeInfoHeader(&infoh)) == infoheadersize);

    zassert(data_out.write(pixdata, outsize) == outsize);
    delete[] pixdata;

    return true;
}

bool ZBMP::read(ZPath path){
    try {
        ZBinary buffer;
        if(ZFile::readBinary(path, buffer) < 54){
            throw ZException("File too small", BMPError::badfile, false);
        }

        decode(buffer);

    } catch(ZException e){
        //ELOG("BMP Read error " << e.code() << ": " << e.what());
        return false;
    }
    return true;
}

bool ZBMP::write(ZPath path){
    try {
        ZBinary out;
        encode(out);

        if(!ZFile::writeBinary(path, out)){
            throw ZException("BMP Write: bad write file");
            return false;
        }
    } catch(ZException e){
        return false;
    }
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
