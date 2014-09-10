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

    bool read(ZPath path);
    bool write(ZPath path) const;

    static ZString libpngVersionInfo();

    ZBitmapRGBA toBitmap() const {
        return bitmap;
    }

private:
    struct PngData {
        png_structp png_ptr;
        png_infop info_ptr;

        png_uint_32 width, height;
        int bit_depth, color_type;

        unsigned char *image_data = NULL;
    };

    struct mainprog_info {
        double gamma;
        long width;
        long height;
        time_t modtime;
        FILE *infile;
        FILE *outfile;
        void *png_ptr;
        void *info_ptr;
        unsigned char *image_data;
        unsigned char **row_pointers;
        char *title;
        char *author;
        char *desc;
        char *copyright;
        char *email;
        char *url;
        int filter;    /* command-line-filter flag, not PNG row filter! */
        int pnmtype;
        int sample_depth;
        int interlaced;
        int have_bg;
        int have_time;
        int have_text;
        jmp_buf jmpbuf;
        unsigned char bg_red;
        unsigned char bg_green;
        unsigned char bg_blue;
    };

private:
    int readpng_init(PngData *data, FILE *infile, unsigned long *pWidth, unsigned long *pHeight);
    unsigned char *readpng_get_image(PngData *data, double display_exponent, int *pChannels, unsigned long *pRowbytes);
    void readpng_cleanup(PngData *data, int free_image_data);

    int writepng_init(mainprog_info *mainprog_ptr);
    int writepng_encode_image(mainprog_info *mainprog_ptr);
    static void writepng_error_handler(png_structp png_ptr, png_const_charp msg);

private:
    ZBitmapRGBA bitmap;

//    int x, y;
//    int width, height, rowbytes;
//    int number_of_passes;
//    png_bytep *row_pointers;
};

}

#endif // ZPNG_H
