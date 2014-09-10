#ifndef ZPNG_H
#define ZPNG_H

#include "ztypes.h"
#include "zimage.h"
//#include "zbitmap.h"
#include "zstring.h"
#include "zpath.h"

#define PNG_DEBUG 3
#include <png.h>

namespace LibChaos {

class ZPNG : public ZImage {
public:
    ZPNG(){

    }
    ZPNG(const ZBitmapRGBA &bmp) : bitmap(bmp){

    }

    int readpng_init(FILE *infile, unsigned long *pWidth, unsigned long *pHeight);
    unsigned char *readpng_get_image(double display_exponent, int *pChannels, unsigned long *pRowbytes);
    void readpng_cleanup(int free_image_data);

    bool read(ZPath path);
    bool write(ZPath path) const;

    //void read_png_file(char *file_name);
    //void write_png_file(char *file_name);
    //void process_file();

    static ZString libpngVersionInfo();

    ZBitmapRGBA toBitmap() const {
        return bitmap;
    }

private:
    ZBitmapRGBA bitmap;

    png_structp png_ptr;
    png_infop info_ptr;

    png_uint_32 width, height;
    int bit_depth, color_type;

    unsigned char *image_data = NULL;

//    int x, y;
//    int width, height, rowbytes;
//    int number_of_passes;
//    png_bytep *row_pointers;
};

}

#endif // ZPNG_H
