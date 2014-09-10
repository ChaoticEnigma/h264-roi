#include "zbmp.h"
#include "zfile.h"

namespace LibChaos {

struct BitmapFileHeader {
    unsigned short bfType       : 16;
    unsigned int bfSize         : 32;
    unsigned short bfReserved1  : 16;
    unsigned short bfReserved2  : 16;
    unsigned int bfOffBits      : 32;
};

struct BitmapInfoHeader {
    unsigned int biSize         : 32;
    unsigned int biWidth        : 32;
    unsigned int biHeight       : 32;
    unsigned short biPlanes     : 16;
    unsigned short biBitCount   : 16;
    unsigned int biCompression  : 32;
    unsigned int biSizeImage    : 32;
    unsigned int biXPelsPerMeter: 32;
    unsigned int biYPelsPerMeter: 32;
    unsigned int biClrUsed      : 32;
    unsigned int biClrImportant : 32;
};

void readFileHeader(const unsigned char *start, BitmapFileHeader *fileh){
    fileh->bfType       = *(unsigned short*)(&start[0]);
    fileh->bfSize       = *(unsigned int*)  (&start[2]);
    fileh->bfReserved1  = *(unsigned short*)(&start[6]);
    fileh->bfReserved2  = *(unsigned short*)(&start[8]);
    fileh->bfOffBits    = *(unsigned int*)  (&start[10]);
}
void readInfoHeader(const unsigned char *start, BitmapInfoHeader *infoh){
    infoh->biSize           = *(unsigned int*)  (&start[0]);
    infoh->biWidth          = *(unsigned int*)  (&start[4]);
    infoh->biHeight         = *(unsigned int*)  (&start[8]);
    infoh->biPlanes         = *(unsigned short*)(&start[12]);
    infoh->biBitCount       = *(unsigned short*)(&start[14]);
    infoh->biCompression    = *(unsigned int*)  (&start[16]);
    infoh->biSizeImage      = *(unsigned int*)  (&start[20]);
    infoh->biXPelsPerMeter  = *(unsigned int*)  (&start[24]);
    infoh->biYPelsPerMeter  = *(unsigned int*)  (&start[28]);
    infoh->biClrUsed        = *(unsigned int*)  (&start[32]);
    infoh->biClrImportant   = *(unsigned int*)  (&start[36]);
}

bool ZBMP::read(ZPath path){
    ZBinary buffer = ZFile::readBinary(path);
    BitmapFileHeader fileh;
    readFileHeader(buffer.raw(), &fileh);
    BitmapInfoHeader infoh;
    readInfoHeader(buffer.raw() + 14, &infoh);

    if(fileh.bfType != 0x4d42){
        throw ZError("Unknown BMP type");
        return false;
    }
    if(infoh.biCompression != BI_RGB){
        throw ZError(ZString("Unsupported BMP compression ") << infoh.biCompression);
        return false;
    }

    zu64 width = infoh.biWidth;
    zu64 height = infoh.biHeight;
    zu64 row = width * sizeof(Pixel);
    zu64 rowsize = row + (4 - (row % 4));

    if(buffer.size() != (54 + rowsize * height)){
        throw ZError("BMP file size mismacth or improperly aligned");
        return false;
    }

    const unsigned char *data = buffer.raw() + fileh.bfOffBits;
    Pixel *pixels = new Pixel[width * height];

    for(zu64 i = 0, ir = height-1; i < height; ++i, --ir){
        for(zu64 j = 0; j < width; ++j){
            zu64 pos = (ir * rowsize) + (j * sizeof(Pixel));
            pixels[i * width + j] = {
                data[pos + 2],
                data[pos + 1],
                data[pos + 0]
            };
        }
    }

    bitmap.load(pixels, width, height);

    delete[] pixels;

    return true;
}

ZBinary writeFileHeader(const BitmapFileHeader *fileh){
    ZBinary out;
    out.fill(0, 14);
    *(unsigned short*)(&out.raw()[0])   = fileh->bfType;
    *(unsigned int*)  (&out.raw()[2])   = fileh->bfSize;
    *(unsigned short*)(&out.raw()[6])   = fileh->bfReserved1;
    *(unsigned short*)(&out.raw()[8])   = fileh->bfReserved2;
    *(unsigned int*)  (&out.raw()[10])  = fileh->bfOffBits;
    return out;
}
ZBinary writeInfoHeader(const BitmapInfoHeader *infoh){
    ZBinary out;
    out.fill(0, 40);
    *(unsigned int*)  (&out.raw()[0])   = infoh->biSize;
    *(unsigned int*)  (&out.raw()[4])   = infoh->biWidth;
    *(unsigned int*)  (&out.raw()[8])   = infoh->biHeight;
    *(unsigned short*)(&out.raw()[12])  = infoh->biPlanes;
    *(unsigned short*)(&out.raw()[14])  = infoh->biBitCount;
    *(unsigned int*)  (&out.raw()[16])  = infoh->biCompression;
    *(unsigned int*)  (&out.raw()[20])  = infoh->biSizeImage;
    *(unsigned int*)  (&out.raw()[24])  = infoh->biXPelsPerMeter;
    *(unsigned int*)  (&out.raw()[28])  = infoh->biYPelsPerMeter;
    *(unsigned int*)  (&out.raw()[32])  = infoh->biClrUsed;
    *(unsigned int*)  (&out.raw()[36])  = infoh->biClrImportant;
    return out;
}

bool ZBMP::write(ZPath path) const {
    ZBinary out;

    zu32 row = sizeof(Pixel) * bitmap.width();
    zu32 rowsize = row + (4 - (row % 4));
    zu32 filesize = 54 + rowsize * bitmap.height();

    BitmapFileHeader fileh;
    memset(&fileh, 0, sizeof(BitmapFileHeader));
    fileh.bfType = 0x4d42;
    fileh.bfSize = filesize;
    fileh.bfOffBits = 54;
    out.concat(writeFileHeader(&fileh));

    BitmapInfoHeader infoh;
    memset(&infoh, 0, sizeof(BitmapInfoHeader));
    infoh.biSize = 40;
    infoh.biWidth = bitmap.width();
    infoh.biHeight = bitmap.height();
    infoh.biPlanes = 1;
    infoh.biBitCount = 24;
    infoh.biCompression = 0;
    out.concat(writeInfoHeader(&infoh));

    unsigned char *pixels = new unsigned char[rowsize * bitmap.height()];
    memset(pixels, 0, rowsize * bitmap.height());

    for(zu64 i = 0, ir = bitmap.height()-1; i < bitmap.height(); ++i, --ir){
        for(zu64 j = 0; j < bitmap.width(); ++j){
            zu64 pos = (i * rowsize) + (j * sizeof(Pixel));
            pixels[pos + 0] = bitmap.buffer()[ir * bitmap.width() + j].b;
            pixels[pos + 1] = bitmap.buffer()[ir * bitmap.width() + j].g;
            pixels[pos + 2] = bitmap.buffer()[ir * bitmap.width() + j].r;
        }
    }

    out.concat(ZBinary(pixels, rowsize * bitmap.height()));

    delete[] pixels;

    return ZFile::writeBinary(path, out);
}

}
