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
    unsigned int biSize             : 32;
    unsigned int biWidth            : 32;
    unsigned int biHeight           : 32;
    unsigned short biPlanes         : 16;
    unsigned short biBitCount       : 16;
    unsigned int biCompression      : 32;
    unsigned int biSizeImage        : 32;
    unsigned int biXPelsPerMeter    : 32;
    unsigned int biYPelsPerMeter    : 32;
    unsigned int biClrUsed          : 32;
    unsigned int biClrImportant     : 32;
};

void readFileHeader(const unsigned char *start, BitmapFileHeader *fileh){
    fileh->bfType       = *(const unsigned short*)(&start[0]);
    fileh->bfSize       = *(const unsigned int*)  (&start[2]);
    fileh->bfReserved1  = *(const unsigned short*)(&start[6]);
    fileh->bfReserved2  = *(const unsigned short*)(&start[8]);
    fileh->bfOffBits    = *(const unsigned int*)  (&start[10]);
}
void readInfoHeader(const unsigned char *start, BitmapInfoHeader *infoh){
    infoh->biSize           = *(const unsigned int*)  (&start[0]);
    infoh->biWidth          = *(const unsigned int*)  (&start[4]);
    infoh->biHeight         = *(const unsigned int*)  (&start[8]);
    infoh->biPlanes         = *(const unsigned short*)(&start[12]);
    infoh->biBitCount       = *(const unsigned short*)(&start[14]);
    infoh->biCompression    = *(const unsigned int*)  (&start[16]);
    infoh->biSizeImage      = *(const unsigned int*)  (&start[20]);
    infoh->biXPelsPerMeter  = *(const unsigned int*)  (&start[24]);
    infoh->biYPelsPerMeter  = *(const unsigned int*)  (&start[28]);
    infoh->biClrUsed        = *(const unsigned int*)  (&start[32]);
    infoh->biClrImportant   = *(const unsigned int*)  (&start[36]);
}

bool ZBMP::read(ZPath path){
    try {
        ZBinary buffer = ZFile::readBinary(path);
        if(buffer.size() < 54){
            throw ZError("File too small", BMPError::badfile, false);
        }

        BitmapFileHeader fileh;
        readFileHeader(buffer.raw(), &fileh);

        if(fileh.bfType != 0x4d42){
            throw ZError("Not a BMP file", BMPError::notabmp, false);
        }
        if(fileh.bfSize != buffer.size()){
            throw ZError("Incorrect file size in file header", BMPError::incorrectsize, false);
        }

        BitmapInfoHeader infoh;
        readInfoHeader(buffer.raw() + 14, &infoh);

        if(infoh.biSize != 40){
            throw ZError("Unsupported info header length", BMPError::badinfoheader, false);
        }
        if(infoh.biCompression != BI_RGB){
            throw ZError(ZString("Unsupported compression: ") << infoh.biCompression, BMPError::badcompression, false);
        }
        if(infoh.biBitCount != 24){
            throw ZError(ZString("Unsupported pixel bit count: ") << infoh.biBitCount, BMPError::badbitcount, false);
        }

        zu64 width = infoh.biWidth;
        zu64 height = infoh.biHeight;
        zu64 row = width * bmp_channels;
        zu64 rowsize = row + (4 - (row % 4));

        if((54 + rowsize * height) != fileh.bfSize){
            throw ZError("Incorrect image height / width or improperly aligned", BMPError::incorrectdimensions, false);
        }

        image.setDimensions(width, height, bmp_channels, 8);

        const unsigned char *data = buffer.raw() + fileh.bfOffBits;
        unsigned char *pixels = new unsigned char[width * height * bmp_channels];

        for(zu64 i = 0, ir = height-1; i < height; ++i, --ir){
            for(zu64 j = 0; j < width; ++j){
                zu64 datpos = (i * rowsize) + (j * image.pixelSize());
                zu64 pixpos = (ir * image.rowSize()) + (j * image.pixelSize());

                pixels[pixpos + 0] = data[datpos + 2];
                pixels[pixpos + 1] = data[datpos + 1];
                pixels[pixpos + 2] = data[datpos + 0];
            }
        }

        image.takeData(pixels);

    } catch(ZError e){
        error = e;
        //ELOG("BMP Read error " << e.code() << ": " << e.what());
        return false;
    }

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

    if(image.channels() != 3 || image.depth() != 8){
        throw ZError("BMP Write: Invalid channels depth");
    }

    zu32 rowsize = image.rowSize() + (4 - (image.rowSize() % 4));
    zu32 filesize = 54 + rowsize * image.height();

    BitmapFileHeader fileh;
    memset(&fileh, 0, sizeof(BitmapFileHeader));
    fileh.bfType = 0x4d42;
    fileh.bfSize = filesize;
    fileh.bfOffBits = 54;
    out.concat(writeFileHeader(&fileh));

    BitmapInfoHeader infoh;
    memset(&infoh, 0, sizeof(BitmapInfoHeader));
    infoh.biSize = 40;
    infoh.biWidth = image.width();
    infoh.biHeight = image.height();
    infoh.biPlanes = 1;
    infoh.biBitCount = 24;
    infoh.biCompression = 0;
    out.concat(writeInfoHeader(&infoh));

    const unsigned char *pixels = image.buffer();
    unsigned char *data = new unsigned char[rowsize * image.height()];
    memset(data, 0, rowsize * image.height());

    for(zu64 i = 0, ir = image.height()-1; i < image.height(); ++i, --ir){
        for(zu64 j = 0; j < image.width(); ++j){
            zu64 datpos = (i * rowsize) + (j * image.pixelSize());
            zu64 pixpos = (ir * image.rowSize()) + (j * image.pixelSize());

            data[datpos + 0] = pixels[pixpos + 2];
            data[datpos + 1] = pixels[pixpos + 1];
            data[datpos + 2] = pixels[pixpos + 0];
        }
    }

    out.concat(ZBinary(data, rowsize * image.height()));

    delete[] data;

    return ZFile::writeBinary(path, out);
}

}
