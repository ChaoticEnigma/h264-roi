#include "zpng.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "zerror.h"

namespace LibChaos {

ZPNG::ZPNG(){

}

ZPNG::ZPNG(ZBitmap bmp){

}

ZBitmap ZPNG::toBitmap() const{

    return ZBitmap();
}

bool ZPNG::read(ZPath path){
    png_uint_32 width, height;

    FILE *infile = fopen(path.str().cc(), "rb");

    unsigned char sig[8];
    fread(sig, 1, 8, infile);
    if(!png_check_sig(sig, 8))
        return false;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr)
        return false;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr){
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;   /* out of memory */
    }

    if(setjmp(png_ptr->jmpbuf)){
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
    image.width(width);
    image.height(height);

    return true;
}

bool ZPNG::write(ZPath path) const {

    return true;
}

//void ZPNG::read_png_file(char *file_name){
//    unsigned char header[8];    // 8 is the maximum size that can be checked

//    /* open file and test for it being a png */
//    FILE *fp = fopen(file_name, "rb");
//    if(!fp)
//        throw ZError(ZString("[read_png_file] File %s could not be opened for reading ") << file_name);

//    fread(header, 1, 8, fp);
//    if(png_sig_cmp(header, 0, 8))
//        throw ZError(ZString("[read_png_file] File %s is not recognized as a PNG file ") << file_name);


//    /* initialize stuff */
//    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

//    if(!png_ptr)
//        throw ZError("[read_png_file] png_create_read_struct failed");

//    info_ptr = png_create_info_struct(png_ptr);
//    if(!info_ptr)
//        throw ZError("[read_png_file] png_create_info_struct failed");

//    if(setjmp(png_jmpbuf(png_ptr)))
//        throw ZError("[read_png_file] Error during init_io");

//    png_init_io(png_ptr, fp);
//    png_set_sig_bytes(png_ptr, 8);

//    png_read_info(png_ptr, info_ptr);

//    width = png_get_image_width(png_ptr, info_ptr);
//    height = png_get_image_height(png_ptr, info_ptr);
//    color_type = png_get_color_type(png_ptr, info_ptr);
//    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

//    number_of_passes = png_set_interlace_handling(png_ptr);
//    png_read_update_info(png_ptr, info_ptr);


//    /* read file */
//    if(setjmp(png_jmpbuf(png_ptr)))
//        throw ZError("[read_png_file] Error during read_image");

//    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);

//    if(bit_depth == 16)
//        rowbytes = width*8;
//    else
//        rowbytes = width*4;

//    for(y=0; y<height; y++)
//        row_pointers[y] = (png_byte*) malloc(rowbytes);

//    png_read_image(png_ptr, row_pointers);

//    fclose(fp);
//}


//void ZPNG::write_png_file(char *file_name){
//    /* create file */
//    FILE *fp = fopen(file_name, "wb");
//    if(!fp)
//        throw ZError(ZString("[write_png_file] File %s could not be opened for writing") << file_name);

//    /* initialize stuff */
//    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

//    if(!png_ptr)
//        throw ZError("[write_png_file] png_create_write_struct failed");

//    info_ptr = png_create_info_struct(png_ptr);
//    if(!info_ptr)
//        throw ZError("[write_png_file] png_create_info_struct failed");

//    if(setjmp(png_jmpbuf(png_ptr)))
//        throw ZError("[write_png_file] Error during init_io");

//    png_init_io(png_ptr, fp);


//    /* write header */
//    if(setjmp(png_jmpbuf(png_ptr)))
//        throw ZError("[write_png_file] Error during writing header");

//    png_set_IHDR(png_ptr, info_ptr, width, height, 8, 6, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

//    png_write_info(png_ptr, info_ptr);


//    /* write bytes */
//    if (setjmp(png_jmpbuf(png_ptr)))
//        throw ZError("[write_png_file] Error during writing bytes");

//    png_write_image(png_ptr, row_pointers);


//    /* end write */
//    if (setjmp(png_jmpbuf(png_ptr)))
//        throw ZError("[write_png_file] Error during end of write");

//    png_write_end(png_ptr, NULL);

//    /* cleanup heap allocation */
//    for (y=0; y<height; y++)
//        free(row_pointers[y]);

//    free(row_pointers);

//    fclose(fp);
//}


//void ZPNG::process_file(){
//    /* Expand any grayscale, RGB, or palette images to RGBA */
//    png_set_expand(png_ptr);

//    /* Reduce any 16-bits-per-sample images to 8-bits-per-sample */
//    png_set_strip_16(png_ptr);

//    for(y=0; y<height; y++){
//        png_byte* row = row_pointers[y];
//        for (x=0; x<width; x++) {
//            png_byte *ptr = &(row[x*4]);
//            //printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n", x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

//            // perform whatever modifications needed, for example to set red value to 0 and green value to the blue one:
//            //ptr[0] = 0;
//            //ptr[1] = ptr[2];
//        }
//    }
//}

ZString  ZPNG::libpngVersionInfo(){
    return ZString("Compiled libpng: ") << PNG_LIBPNG_VER_STRING << ", Using libpng: " << png_libpng_ver << ", Compiled zlib: " << ZLIB_VERSION << ", Using zlib: " << zlib_version;
}

}
