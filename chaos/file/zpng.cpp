#include "zpng.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "zerror.h"
#include "zlog.h"

namespace LibChaos {

int ZPNG::readpng_init(FILE *infile, unsigned long *pWidth, unsigned long *pHeight){
    unsigned char sig[8];

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */

    fread(sig, 1, 8, infile);
    if (!png_check_sig(sig, 8))
        return 1;   /* bad signature */

    /* could pass pointers to user-defined error handlers instead of NULLs: */

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }

    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */

    /* alternatively, could make separate calls to png_get_image_width(),
     * etc., but want bit_depth and color_type for later [don't care about
     * compression_type and filter_type => NULLs] */

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
      NULL, NULL, NULL);
    *pWidth = width;
    *pHeight = height;

    /* OK, that's all we need for now; return happy */
    return 0;
}

unsigned char *ZPNG::readpng_get_image(double display_exponent, int *pChannels, unsigned long *pRowbytes){
    double  gamma;
    png_uint_32  i, rowbytes;
    png_bytepp  row_pointers = NULL;

    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    /* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
     * transparency chunks to full alpha channel; strip 16-bit-per-sample
     * images to 8 bits per sample; and convert grayscale to RGB[A] */

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    /* unlike the example in the libpng documentation, we have *no* idea where
     * this file may have come from--so if it doesn't have a file gamma, don't
     * do any correction ("do no harm") */

    if(png_get_gAMA(png_ptr, info_ptr, &gamma))
        png_set_gamma(png_ptr, display_exponent, gamma);

    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */

    png_read_update_info(png_ptr, info_ptr);

    *pRowbytes = rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    *pChannels = (int)png_get_channels(png_ptr, info_ptr);

    if ((image_data = (unsigned char *)malloc(rowbytes*height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
        return NULL;
    }

    //Trace((stderr, "readpng_get_image:  channels = %d, rowbytes = %ld, height = %ld\n", *pChannels, rowbytes, height));

    /* set the individual row_pointers to point at the correct offsets */

    for (i = 0;  i < height;  ++i)
        row_pointers[i] = image_data + i*rowbytes;

    /* now we can go ahead and just read the whole image */

    png_read_image(png_ptr, row_pointers);

    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

    free(row_pointers);
    row_pointers = NULL;

    png_read_end(png_ptr, NULL);

    return image_data;
}


void ZPNG::readpng_cleanup(int free_image_data){
    if (free_image_data && image_data) {
        free(image_data);
        image_data = NULL;
    }

    if (png_ptr && info_ptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        png_ptr = NULL;
        info_ptr = NULL;
    }
}

bool ZPNG::read(ZPath path){
//    png_uint_32 width, height;

//    FILE *infile = fopen(path.str().cc(), "rb");

//    unsigned char sig[8];
//    fread(sig, 1, 8, infile);
//    if(!png_check_sig(sig, 8))
//        return false;

//    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//    if(!png_ptr)
//        return false;   /* out of memory */

//    info_ptr = png_create_info_struct(png_ptr);
//    if(!info_ptr){
//        png_destroy_read_struct(&png_ptr, NULL, NULL);
//        return false;   /* out of memory */
//    }

//    if(setjmp(png_ptr->jmpbuf)){
//        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
//        return false;
//    }

//    png_init_io(png_ptr, infile);
//    png_set_sig_bytes(png_ptr, 8);
//    png_read_info(png_ptr, info_ptr);

//    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
//    bitmap.width(width);
//    bitmap.height(height);

    FILE *infile = fopen(path.str().cc(), "rb");
    int channels;
    unsigned long w, h, row;
    readpng_init(infile, &w, &h);
    unsigned char *data = readpng_get_image(1.0, &channels, &row);
    ZBitmapRGBA image((PixelRGBA*)data, w, h);
    bitmap = image;

    LOG("Image: " << sizeof(PixelRGBA) << " " << sizeof(PixelRGB) << " " << bitmap.pixels());

    readpng_cleanup(1);

    return true;
}

bool ZPNG::write(ZPath path) const {

    return true;
}



ZString ZPNG::libpngVersionInfo(){
    return ZString("Compiled libpng: ") << PNG_LIBPNG_VER_STRING << ", Using libpng: " << png_libpng_ver << ", Compiled zlib: " << ZLIB_VERSION << ", Using zlib: " << zlib_version;
}

}
