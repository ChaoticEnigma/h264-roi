#include "zpng.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "zerror.h"
#include "zlog.h"
#include "zfile.h"

//#define TEXT_TITLE    0x01
//#define TEXT_AUTHOR   0x02
//#define TEXT_DESC     0x04
//#define TEXT_COPY     0x08
//#define TEXT_EMAIL    0x10
//#define TEXT_URL      0x20

//#define TEXT_TITLE_OFFSET        0
//#define TEXT_AUTHOR_OFFSET      72
//#define TEXT_COPY_OFFSET     (2*72)
//#define TEXT_EMAIL_OFFSET    (3*72)
//#define TEXT_URL_OFFSET      (4*72)
//#define TEXT_DESC_OFFSET     (5*72)

namespace LibChaos {

bool ZPNG::read(ZPath path){
    FILE *infile = fopen(path.str().cc(), "rb");
    PngReadData *data = new PngReadData;

    unsigned long width, height;
    int result = readpng_init(data, infile, &width, &height);
    if(result != 0){
        fclose(infile);
        readpng_cleanup(data);
        delete data;
        error = ZError("readpng_init failed", 1, false);
        return false;
    }

//    if(data->info_ptr->num_text == 0)
//        LOG("No PNG Text");
    for(int i = 0; i < data->info_ptr->num_text; ++i){
        LOG(data->info_ptr->text[i].key << ": " << data->info_ptr->text[i].text);
        text.push(data->info_ptr->text[i].key, data->info_ptr->text[i].text);
    }

    unsigned long row;
    unsigned char *pixels = readpng_get_image(data, 1.0, &channels, &row);
    if(pixels == NULL){
        fclose(infile);
        readpng_cleanup(data);
        delete data;
        error = ZError("readpng_get_image failed", 1, false);
        return false;
    }

    LOG(path.last() <<" Channels: " << channels << ", " << width << "," << height << ", " << row);
    if(channels == 3){
        ZBitmapRGB tmp((PixelRGB*)pixels, width, height);
        bitmap = tmp.recast<PixelRGBA>(0);
    } else if(channels == 4){
        bitmap = ZBitmapRGBA((PixelRGBA*)pixels, width, height);
    } else {
        fclose(infile);
        readpng_cleanup(data);
        delete data;
        error = ZError("unsupported channel count", 1, false);
        return false;
    }

    fclose(infile);
    readpng_cleanup(data);
    delete data;

    return true;
}

bool ZPNG::write(ZPath path){
    ZFile file(path, ZFile::writeonly);

    PngWriteData *data = new PngWriteData;
    data->outfile = file.fp();
    data->image_data = NULL;
    data->row_pointers = NULL;
    data->bit_depth = 8;
    data->have_bg = 0;
    data->have_time = 0;
    data->gamma = 0.0;
    data->color_type = PNG_COLOR_TYPE_RGB_ALPHA;

    data->interlaced = 1;

    int result = writepng_init(data, text);
    if(result != 0){
        error = ZError("writepng_init failed", 1, false);
        delete data;
        return false;
    }

    if(data->interlaced){

        data->row_pointers = new unsigned char*[bitmap.height() * sizeof(unsigned char *)];

        for(long i = 0; i < bitmap.height(); ++i){
            data->row_pointers[i] = (unsigned char *)bitmap.buffer() + (i * bitmap.width() * sizeof(Pixel32));
        }

        result = writepng_encode_image(data);
        if(result != 0){
            error = ZError("writepng_encode_image failed", 1, false);
            writepng_cleanup(data);
            delete data;
            return false;
        }

    } else {

        for(zu64 i = 0; i < bitmap.height(); ++i){
            data->image_data = (unsigned char *)bitmap.buffer() + (i * bitmap.width() * sizeof(Pixel32));
            result = writepng_encode_row(data);
            if(result != 0){
                error = ZError("writepng_encode_row failed", 1, false);
                writepng_cleanup(data);
                delete data;
                return false;
            }
        }

        result = writepng_encode_finish(data);
        if(result != 0){
            error = ZError("writepng_encode_finish failed", 1, false);
            writepng_cleanup(data);
            delete data;
            return false;
        }

    }

    writepng_cleanup(data);
    delete data;
    return true;
}

ZString ZPNG::libpngVersionInfo(){
    return ZString("Compiled libpng: ") << PNG_LIBPNG_VER_STRING << ", Using libpng: " << png_libpng_ver << ", Compiled zlib: " << ZLIB_VERSION << ", Using zlib: " << zlib_version;
}

// ////////////////////////////////////////////////////////////////////////////
// Private
// ////////////////////////////////////////////////////////////////////////////

int ZPNG::readpng_init(PngReadData *data, FILE *infile, unsigned long *pWidth, unsigned long *pHeight){
    unsigned char sig[8];

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */
    fread(sig, 1, 8, infile);
    if(!png_check_sig(sig, 8))
        return 1;   /* bad signature */

    /* could pass pointers to user-defined error handlers instead of NULLs: */
    data->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, readpng_error_handler, readpng_warning_handler);
    if(!data->png_ptr)
        return 4;   /* out of memory */

    data->info_ptr = png_create_info_struct(data->png_ptr);
    if(!data->info_ptr){
        png_destroy_read_struct(&data->png_ptr, NULL, NULL);
        data->png_ptr = NULL;
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

int ZPNG::readpng_get_bgcolor(PngReadData *data, unsigned char *red, unsigned char *green, unsigned char *blue){
    png_color_16p pBackground;

    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */
    if(setjmp(png_jmpbuf(data->png_ptr))){
        png_destroy_read_struct(&data->png_ptr, &data->info_ptr, NULL);
        return 2;
    }

    if(!png_get_valid(data->png_ptr, data->info_ptr, PNG_INFO_bKGD))
        return 1;

    /* it is not obvious from the libpng documentation, but this function
     * takes a pointer to a pointer, and it always returns valid red, green
     * and blue values, regardless of color_type: */
    png_get_bKGD(data->png_ptr, data->info_ptr, &pBackground);

    /* however, it always returns the raw bKGD data, regardless of any
     * bit-depth transformations, so check depth and adjust if necessary */
    if(data->bit_depth == 16){
        *red   = pBackground->red   >> 8;
        *green = pBackground->green >> 8;
        *blue  = pBackground->blue  >> 8;
    } else if(data->color_type == PNG_COLOR_TYPE_GRAY && data->bit_depth < 8){
        if (data->bit_depth == 1)
            *red = *green = *blue = pBackground->gray ? 255 : 0;
        else if(data->bit_depth == 2)
            *red = *green = *blue = (255 / 3) * pBackground->gray;
        else /* bit_depth == 4 */
            *red = *green = *blue = (255 / 15) * pBackground->gray;
    } else {
        *red   = (unsigned char)pBackground->red;
        *green = (unsigned char)pBackground->green;
        *blue  = (unsigned char)pBackground->blue;
    }

    return 0;
}

unsigned char *ZPNG::readpng_get_image(PngReadData *data, double display_exponent, int *pChannels, unsigned long *pRowbytes){
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

void ZPNG::readpng_cleanup(PngReadData *data){
    if(data->image_data){
        free(data->image_data);
        data->image_data = NULL;
    }

    if(data->png_ptr){
        png_destroy_read_struct(&data->png_ptr, NULL, NULL);
        data->png_ptr = NULL;
    }

    if(data->info_ptr){
        png_destroy_read_struct(NULL, &data->info_ptr, NULL);
        data->info_ptr = NULL;
    }
}

void ZPNG::readpng_warning_handler(png_struct *png_ptr, png_const_charp msg){
    ELOG(ZLog::this_thread << "libpng read warning: " << msg);
}
void ZPNG::readpng_error_handler(png_struct *png_ptr, png_const_charp msg){
    ELOG(ZLog::this_thread << "libpng read error: " << msg);
    longjmp(png_jmpbuf(png_ptr), 1);
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ZPNG::writepng_init(PngWriteData *data, const AsArZ &texts){
    int color_type, interlace_type;

    /* could also replace libpng warning-handler (final NULL), but no need: */
    data->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, data, writepng_error_handler, NULL);
    if (!data->png_ptr)
        return 4;   /* out of memory */

    data->info_ptr = png_create_info_struct(data->png_ptr);
    if (!data->info_ptr) {
        png_destroy_write_struct(&data->png_ptr, NULL);
        return 4;   /* out of memory */
    }

    /* setjmp() must be called in every function that calls a PNG-writing
     * libpng function, unless an alternate error handler was installed--
     * but compatible error handlers must either use longjmp() themselves
     * (as in this program) or exit immediately, so here we go: */
    if (setjmp(data->jmpbuf)) {
        png_destroy_write_struct(&data->png_ptr, &data->info_ptr);
        return 2;
    }

    /* make sure outfile is (re)opened in BINARY mode */
    png_init_io(data->png_ptr, data->outfile);

    /* set the compression levels--in general, always want to leave filtering
     * turned on (except for palette images) and allow all of the filters,
     * which is the default; want 32K zlib window, unless entire image buffer
     * is 16K or smaller (unknown here)--also the default; usually want max
     * compression (NOT the default); and remaining compression flags should
     * be left alone */
    png_set_compression_level(data->png_ptr, Z_BEST_COMPRESSION);
/*
    >> this is default for no filtering; Z_FILTERED is default otherwise:
    png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
    >> these are all defaults:
    png_set_compression_mem_level(png_ptr, 8);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_method(png_ptr, 8);
 */

    interlace_type = data->interlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    png_set_IHDR(data->png_ptr, data->info_ptr, data->width, data->height, data->bit_depth, color_type, interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    if (data->gamma > 0.0)
        png_set_gAMA(data->png_ptr, data->info_ptr, data->gamma);

    if (data->have_bg) {   /* we know it's RGBA, not gray+alpha */
        png_color_16  background;

        background.red = data->bg_red;
        background.green = data->bg_green;
        background.blue = data->bg_blue;
        png_set_bKGD(data->png_ptr, data->info_ptr, &background);
    }

    if (data->have_time) {
        png_time  modtime;

        png_convert_from_time_t(&modtime, data->modtime);
        png_set_tIME(data->png_ptr, data->info_ptr, &modtime);
    }

    // Add text strings to PNG
    png_text *pngtext = new png_text[texts.size()];
    for(zu64 i = 0; i < texts.size(); ++i){
        pngtext[i].compression = PNG_TEXT_COMPRESSION_NONE;
        pngtext[i].key = (char *)texts.key(i).cc();
        pngtext[i].text = (char *)texts.val(i).cc();
    }
    png_set_text(data->png_ptr, data->info_ptr, pngtext, texts.size());
    delete[] pngtext;

//    if (mainprog_ptr->have_text) {
//        png_text text[6];
//        int num_text = 0;

//        if(mainprog_ptr->have_text & TEXT_TITLE){
//            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
//            text[num_text].key = "Title";
//            text[num_text].text = mainprog_ptr->title;
//            ++num_text;
//        }
//        if(mainprog_ptr->have_text & TEXT_AUTHOR){
//            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
//            text[num_text].key = "Author";
//            text[num_text].text = mainprog_ptr->author;
//            ++num_text;
//        }
//        if(mainprog_ptr->have_text & TEXT_DESC){
//            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
//            text[num_text].key = "Description";
//            text[num_text].text = mainprog_ptr->desc;
//            ++num_text;
//        }
//        if(mainprog_ptr->have_text & TEXT_COPY){
//            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
//            text[num_text].key = "Copyright";
//            text[num_text].text = mainprog_ptr->copyright;
//            ++num_text;
//        }
//        if(mainprog_ptr->have_text & TEXT_EMAIL){
//            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
//            text[num_text].key = "E-mail";
//            text[num_text].text = mainprog_ptr->email;
//            ++num_text;
//        }
//        if(mainprog_ptr->have_text & TEXT_URL){
//            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
//            text[num_text].key = "URL";
//            text[num_text].text = mainprog_ptr->url;
//            ++num_text;
//        }
//        png_set_text(png_ptr, info_ptr, text, num_text);
//    }

    /* write all chunks up to (but not including) first IDAT */
    png_write_info(data->png_ptr, data->info_ptr);

    /* if we wanted to write any more text info *after* the image data, we
     * would set up text struct(s) here and call png_set_text() again, with
     * just the new data; png_set_tIME() could also go here, but it would
     * have no effect since we already called it above (only one tIME chunk
     * allowed) */

    /* set up the transformations:  for now, just pack low-bit-depth pixels
     * into bytes (one, two or four pixels per byte) */
    png_set_packing(data->png_ptr);
/*  png_set_shift(png_ptr, &sig_bit);  to scale low-bit-depth values */

    return 0;
}

int ZPNG::writepng_encode_image(PngWriteData *data){
    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */
    if (setjmp(data->jmpbuf)) {
        png_destroy_write_struct(&data->png_ptr, &data->info_ptr);
        data->png_ptr = NULL;
        data->info_ptr = NULL;
        return 2;
    }

    /* and now we just write the whole image; libpng takes care of interlacing
     * for us */
    png_write_image(data->png_ptr, data->row_pointers);

    /* since that's it, we also close out the end of the PNG file now--if we
     * had any text or time info to write after the IDATs, second argument
     * would be info_ptr, but we optimize slightly by sending NULL pointer: */
    png_write_end(data->png_ptr, NULL);

    return 0;
}

int ZPNG::writepng_encode_row(PngWriteData *data){
    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */
    if(setjmp(data->jmpbuf)){
        png_destroy_write_struct(&data->png_ptr, &data->info_ptr);
        data->png_ptr = NULL;
        data->info_ptr = NULL;
        return 2;
    }

    /* image_data points at our one row of image data */
    png_write_row(data->png_ptr, data->image_data);

    return 0;
}

int ZPNG::writepng_encode_finish(PngWriteData *data){
    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */
    if (setjmp(data->jmpbuf)) {
        png_destroy_write_struct(&data->png_ptr, &data->info_ptr);
        data->png_ptr = NULL;
        data->info_ptr = NULL;
        return 2;
    }

    /* close out PNG file; if we had any text or time info to write after
     * the IDATs, second argument would be info_ptr: */
    png_write_end(data->png_ptr, NULL);

    return 0;
}

void ZPNG::writepng_cleanup(PngWriteData *mainprog_ptr){
    if(mainprog_ptr->png_ptr){
        png_destroy_write_struct(&mainprog_ptr->png_ptr, NULL);
        mainprog_ptr->png_ptr = NULL;
    }
    if(mainprog_ptr->info_ptr){
        png_destroy_write_struct(NULL, &mainprog_ptr->info_ptr);
        mainprog_ptr->info_ptr = NULL;
    }
}

void ZPNG::writepng_error_handler(png_struct *png_ptr, png_const_charp msg){
    ELOG("libpng write error: " << msg);

    PngWriteData *mainprog_ptr = (PngWriteData *)png_get_error_ptr(png_ptr);
    if(mainprog_ptr == NULL){
        // FUCK.
        ELOG(ZLog::this_thread << "libpng write severe error: jmpbuf not recoverable; terminating.");
        exit(99);
    }
    longjmp(mainprog_ptr->jmpbuf, 1);
}

}
