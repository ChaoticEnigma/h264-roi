#include "zpng.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "zerror.h"
#include "zlog.h"

#define TEXT_TITLE    0x01
#define TEXT_AUTHOR   0x02
#define TEXT_DESC     0x04
#define TEXT_COPY     0x08
#define TEXT_EMAIL    0x10
#define TEXT_URL      0x20

#define TEXT_TITLE_OFFSET        0
#define TEXT_AUTHOR_OFFSET      72
#define TEXT_COPY_OFFSET     (2*72)
#define TEXT_EMAIL_OFFSET    (3*72)
#define TEXT_URL_OFFSET      (4*72)
#define TEXT_DESC_OFFSET     (5*72)

namespace LibChaos {

bool ZPNG::read(ZPath path){
    FILE *infile = fopen(path.str().cc(), "rb");
    int channels;
    unsigned long wdth, hght, row;
    PngData *data = new PngData;
    readpng_init(data, infile, &wdth, &hght);
    unsigned char *pixels = readpng_get_image(data, 1.0, &channels, &row);
    bitmap = ZBitmapRGBA((PixelRGBA*)pixels, wdth, hght);
    fclose(infile);
    readpng_cleanup(data, 1);
    delete data;
    return true;
}

bool ZPNG::write(ZPath path) const {

    return true;
}

ZString ZPNG::libpngVersionInfo(){
    return ZString("Compiled libpng: ") << PNG_LIBPNG_VER_STRING << ", Using libpng: " << png_libpng_ver << ", Compiled zlib: " << ZLIB_VERSION << ", Using zlib: " << zlib_version;
}

// ////////////////////////////////////////////////////////////////////////////
// Private
// ////////////////////////////////////////////////////////////////////////////

int ZPNG::readpng_init(PngData *data, FILE *infile, unsigned long *pWidth, unsigned long *pHeight){
    unsigned char sig[8];

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */
    fread(sig, 1, 8, infile);
    if(!png_check_sig(sig, 8))
        return 1;   /* bad signature */

    /* could pass pointers to user-defined error handlers instead of NULLs: */
    data->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!data->png_ptr)
        return 4;   /* out of memory */

    data->info_ptr = png_create_info_struct(data->png_ptr);
    if(!data->info_ptr){
        png_destroy_read_struct(&data->png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }

    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */

    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */
    if(setjmp(png_jmpbuf(data->png_ptr))){
        png_destroy_read_struct(&data->png_ptr, &data->info_ptr, NULL);
        return 2;
    }

    png_init_io(data->png_ptr, infile);
    png_set_sig_bytes(data->png_ptr, 8);  /* we already read the 8 signature bytes */

    png_read_info(data->png_ptr, data->info_ptr);  /* read all PNG info up to image data */

    /* alternatively, could make separate calls to png_get_image_width(),
     * etc., but want bit_depth and color_type for later [don't care about
     * compression_type and filter_type => NULLs] */
    png_get_IHDR(data->png_ptr, data->info_ptr, &data->width, &data->height, &data->bit_depth, &data->color_type, NULL, NULL, NULL);
    *pWidth = data->width;
    *pHeight = data->height;

    /* OK, that's all we need for now; return happy */
    return 0;
}

unsigned char *ZPNG::readpng_get_image(PngData *data, double display_exponent, int *pChannels, unsigned long *pRowbytes){
    double  gamma;
    png_uint_32  i, rowbytes;
    png_bytepp  row_pointers = NULL;

    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */
    if(setjmp(png_jmpbuf(data->png_ptr))){
        png_destroy_read_struct(&data->png_ptr, &data->info_ptr, NULL);
        return NULL;
    }

    /* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
     * transparency chunks to full alpha channel; strip 16-bit-per-sample
     * images to 8 bits per sample; and convert grayscale to RGB[A] */
    if(data->color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(data->png_ptr);
    if(data->color_type == PNG_COLOR_TYPE_GRAY && data->bit_depth < 8)
        png_set_expand(data->png_ptr);
    if(png_get_valid(data->png_ptr, data->info_ptr, PNG_INFO_tRNS))
        png_set_expand(data->png_ptr);
    if(data->bit_depth == 16)
        png_set_strip_16(data->png_ptr);
    if(data->color_type == PNG_COLOR_TYPE_GRAY ||
        data->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(data->png_ptr);

    /* unlike the example in the libpng documentation, we have *no* idea where
     * this file may have come from--so if it doesn't have a file gamma, don't
     * do any correction ("do no harm") */
    if(png_get_gAMA(data->png_ptr, data->info_ptr, &gamma))
        png_set_gamma(data->png_ptr, display_exponent, gamma);

    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */
    png_read_update_info(data->png_ptr, data->info_ptr);

    *pRowbytes = rowbytes = png_get_rowbytes(data->png_ptr, data->info_ptr);
    *pChannels = (int)png_get_channels(data->png_ptr, data->info_ptr);

    if((data->image_data = (unsigned char *)malloc(rowbytes * data->height)) == NULL){
        png_destroy_read_struct(&data->png_ptr, &data->info_ptr, NULL);
        return NULL;
    }
    if((row_pointers = (png_bytepp)malloc(data->height * sizeof(png_bytep))) == NULL){
        png_destroy_read_struct(&data->png_ptr, &data->info_ptr, NULL);
        free(data->image_data);
        data->image_data = NULL;
        return NULL;
    }

    /* set the individual row_pointers to point at the correct offsets */
    for(i = 0;  i < data->height;  ++i)
        row_pointers[i] = data->image_data + i*rowbytes;

    /* now we can go ahead and just read the whole image */
    png_read_image(data->png_ptr, row_pointers);

    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */
    free(row_pointers);
    row_pointers = NULL;

    png_read_end(data->png_ptr, NULL);

    return data->image_data;
}

void ZPNG::readpng_cleanup(PngData *data, int free_image_data){
    if(free_image_data && data->image_data){
        free(data->image_data);
        data->image_data = NULL;
    }

    if(data->png_ptr && data->info_ptr){
        png_destroy_read_struct(&data->png_ptr, &data->info_ptr, NULL);
        data->png_ptr = NULL;
        data->info_ptr = NULL;
    }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ZPNG::writepng_init(mainprog_info *mainprog_ptr){
    png_structp  png_ptr;       /* note:  temporary variables! */
    png_infop  info_ptr;
    int color_type, interlace_type;

    /* could also replace libpng warning-handler (final NULL), but no need: */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, mainprog_ptr, writepng_error_handler, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        return 4;   /* out of memory */
    }

    /* setjmp() must be called in every function that calls a PNG-writing
     * libpng function, unless an alternate error handler was installed--
     * but compatible error handlers must either use longjmp() themselves
     * (as in this program) or exit immediately, so here we go: */
    if (setjmp(mainprog_ptr->jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 2;
    }

    /* make sure outfile is (re)opened in BINARY mode */
    png_init_io(png_ptr, mainprog_ptr->outfile);

    /* set the compression levels--in general, always want to leave filtering
     * turned on (except for palette images) and allow all of the filters,
     * which is the default; want 32K zlib window, unless entire image buffer
     * is 16K or smaller (unknown here)--also the default; usually want max
     * compression (NOT the default); and remaining compression flags should
     * be left alone */
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
/*
    >> this is default for no filtering; Z_FILTERED is default otherwise:
    png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
    >> these are all defaults:
    png_set_compression_mem_level(png_ptr, 8);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_method(png_ptr, 8);
 */

    /* set the image parameters appropriately */
    if (mainprog_ptr->pnmtype == 5)
        color_type = PNG_COLOR_TYPE_GRAY;
    else if (mainprog_ptr->pnmtype == 6)
        color_type = PNG_COLOR_TYPE_RGB;
    else if (mainprog_ptr->pnmtype == 8)
        color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    else {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 11;
    }

    interlace_type = mainprog_ptr->interlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    png_set_IHDR(png_ptr, info_ptr, mainprog_ptr->width, mainprog_ptr->height, mainprog_ptr->sample_depth, color_type, interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    if (mainprog_ptr->gamma > 0.0)
        png_set_gAMA(png_ptr, info_ptr, mainprog_ptr->gamma);

    if (mainprog_ptr->have_bg) {   /* we know it's RGBA, not gray+alpha */
        png_color_16  background;

        background.red = mainprog_ptr->bg_red;
        background.green = mainprog_ptr->bg_green;
        background.blue = mainprog_ptr->bg_blue;
        png_set_bKGD(png_ptr, info_ptr, &background);
    }

    if (mainprog_ptr->have_time) {
        png_time  modtime;

        png_convert_from_time_t(&modtime, mainprog_ptr->modtime);
        png_set_tIME(png_ptr, info_ptr, &modtime);
    }

    if (mainprog_ptr->have_text) {
        png_text  text[6];
        int  num_text = 0;

        if(mainprog_ptr->have_text & TEXT_TITLE){
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Title";
            text[num_text].text = mainprog_ptr->title;
            ++num_text;
        }
        if(mainprog_ptr->have_text & TEXT_AUTHOR){
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Author";
            text[num_text].text = mainprog_ptr->author;
            ++num_text;
        }
        if(mainprog_ptr->have_text & TEXT_DESC){
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Description";
            text[num_text].text = mainprog_ptr->desc;
            ++num_text;
        }
        if(mainprog_ptr->have_text & TEXT_COPY){
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Copyright";
            text[num_text].text = mainprog_ptr->copyright;
            ++num_text;
        }
        if(mainprog_ptr->have_text & TEXT_EMAIL){
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "E-mail";
            text[num_text].text = mainprog_ptr->email;
            ++num_text;
        }
        if(mainprog_ptr->have_text & TEXT_URL){
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "URL";
            text[num_text].text = mainprog_ptr->url;
            ++num_text;
        }
        png_set_text(png_ptr, info_ptr, text, num_text);
    }

    /* write all chunks up to (but not including) first IDAT */
    png_write_info(png_ptr, info_ptr);

    /* if we wanted to write any more text info *after* the image data, we
     * would set up text struct(s) here and call png_set_text() again, with
     * just the new data; png_set_tIME() could also go here, but it would
     * have no effect since we already called it above (only one tIME chunk
     * allowed) */

    /* set up the transformations:  for now, just pack low-bit-depth pixels
     * into bytes (one, two or four pixels per byte) */
    png_set_packing(png_ptr);
/*  png_set_shift(png_ptr, &sig_bit);  to scale low-bit-depth values */

    /* make sure we save our pointers for use in writepng_encode_image() */
    mainprog_ptr->png_ptr = png_ptr;
    mainprog_ptr->info_ptr = info_ptr;

    /* OK, that's all we need to do for now; return happy */
    return 0;
}

int ZPNG::writepng_encode_image(mainprog_info *mainprog_ptr){
    png_structp png_ptr = (png_structp)mainprog_ptr->png_ptr;
    png_infop info_ptr = (png_infop)mainprog_ptr->info_ptr;


    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */

    if (setjmp(mainprog_ptr->jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        mainprog_ptr->png_ptr = NULL;
        mainprog_ptr->info_ptr = NULL;
        return 2;
    }


    /* and now we just write the whole image; libpng takes care of interlacing
     * for us */

    png_write_image(png_ptr, mainprog_ptr->row_pointers);


    /* since that's it, we also close out the end of the PNG file now--if we
     * had any text or time info to write after the IDATs, second argument
     * would be info_ptr, but we optimize slightly by sending NULL pointer: */

    png_write_end(png_ptr, NULL);

    return 0;
}

void ZPNG::writepng_error_handler(png_structp png_ptr, png_const_charp msg){
    mainprog_info  *mainprog_ptr;

    /* This function, aside from the extra step of retrieving the "error
     * pointer" (below) and the fact that it exists within the application
     * rather than within libpng, is essentially identical to libpng's
     * default error handler.  The second point is critical:  since both
     * setjmp() and longjmp() are called from the same code, they are
     * guaranteed to have compatible notions of how big a jmp_buf is,
     * regardless of whether _BSD_SOURCE or anything else has (or has not)
     * been defined. */

    fprintf(stderr, "writepng libpng error: %s\n", msg);
    fflush(stderr);

    mainprog_ptr = (mainprog_info *)png_get_error_ptr(png_ptr);
    if (mainprog_ptr == NULL) {         /* we are completely hosed now */
        fprintf(stderr,
          "writepng severe error:  jmpbuf not recoverable; terminating.\n");
        fflush(stderr);
        exit(99);
    }

    longjmp(mainprog_ptr->jmpbuf, 1);
}

}
