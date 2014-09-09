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
    ZPNG();
    ZPNG(ZBitmap bmp);

    bool read(ZPath path);
    bool write(ZPath path) const;

    //void read_png_file(char *file_name);
    //void write_png_file(char *file_name);
    //void process_file();

    ZString libpngVersionInfo();

    ZBitmap toBitmap() const;

private:
    ZBitmap image;

    png_structp png_ptr;
    png_infop info_ptr;

    int color_type;
    int bit_depth;

//    int x, y;
//    int width, height, rowbytes;
//    int number_of_passes;
//    png_bytep *row_pointers;
};

}

#endif // ZPNG_H
