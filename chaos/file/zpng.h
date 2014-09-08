#ifndef ZPNG_H
#define ZPNG_H

#include "ztypes.h"
//#include "zbitmap.h"

#define PNG_DEBUG 3
#include <png.h>

namespace LibChaos {

class ZPNG {
public:
    ZPNG();

    void read_png_file(char *file_name);
    void write_png_file(char *file_name);
    void process_file();

private:
    //ZBitmap image;

    int x, y;

    int width, height, rowbytes;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
};

}

#endif // ZPNG_H
