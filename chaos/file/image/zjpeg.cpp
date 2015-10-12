/*******************************************************************************
**                                  LibChaos                                  **
**                                  zjpeg.cpp                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zjpeg.h"
#include "zfile.h"
#include "zexception.h"

#include <stdio.h>
#include <jpeglib.h>

#define JPEG_SIG 0xFFD8FFE0

namespace LibChaos {

int readjpeg(ZBinary &in, ZImage *out);

ZJPEG::ZJPEG(ZImage *image) : _image(image){
    quality = 100;
}

bool ZJPEG::isJPEG(const ZBinary &data){
    return (ZBinary::decbe32(data.raw()) == JPEG_SIG);
}

bool ZJPEG::decode(ZReader *input){

    return false;
}

bool ZJPEG::encode(ZWriter *output){

    return false;
}

struct my_error_mgr {
    struct jpeg_error_mgr pub;  /* "public" fields */

    //jmp_buf setjmp_buffer;  /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void) my_error_exit(j_common_ptr cinfo){
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    //longjmp(myerr->setjmp_buffer, 1);
    throw ZException("JPEG Error");
}

// JPEG Interface from libjpeg's example.c

int readjpeg(ZBinary &in, ZImage *out){
    /* This struct contains the JPEG decompression parameters and pointers to
     * working space (which is allocated as needed by the JPEG library).
     */
    struct jpeg_decompress_struct cinfo;

    /* We use our private extension JPEG error handler.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    struct my_error_mgr jerr;

    /* More stuff */
    JSAMPARRAY buffer;  /* Output row buffer */
    int row_stride;     /* physical row width in output buffer */

    /* Step 1: allocate and initialize JPEG decompression object */

    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
//    if(setjmp(jerr.setjmp_buffer)) {
//        /* If we get here, the JPEG code has signaled an error.
//         * We need to clean up the JPEG object, close the input file, and return.
//         */
//        jpeg_destroy_decompress(&cinfo);
//        fclose(infile);
//        return 1;
//    }

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&cinfo);

    /* Step 2: specify data source (eg, a file) */

    //jpeg_stdio_src(&cinfo, infile);
    jpeg_mem_src(&cinfo, in.raw(), in.size());

    /* Step 3: read file parameters with jpeg_read_header() */

    jpeg_read_header(&cinfo, TRUE);
    /* We can ignore the return value from jpeg_read_header since
     *   (a) suspension is not possible with the stdio data source, and
     *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
     * See libjpeg.txt for more info.
     */

    /* Step 4: set parameters for decompression */

    /* In this example, we don't need to change any of the defaults set by
     * jpeg_read_header(), so we do nothing here.
     */

    /* Step 5: Start decompressor */

    jpeg_start_decompress(&cinfo);
    /* We can ignore the return value since suspension is not possible
     * with the stdio data source.
     */

    out->setWidth(cinfo.output_width);
    out->setHeight(cinfo.output_width);
    out->setChannels(cinfo.out_color_components);
    //out->setDimensions(cinfo.output_width, cinfo.output_height, cinfo.output_components);

    if(cinfo.out_color_space != JCS_RGB){
        throw ZException("JPEG not RGB");
    }

    /* We may need to do some setup of our own at this point before reading
     * the data.  After jpeg_start_decompress() we have the correct scaled
     * output image dimensions available, as well as the output colormap
     * if we asked for color quantization.
     * In this example, we need to make an output work buffer of the right size.
     */
    /* JSAMPLEs per row in output buffer */
    row_stride = cinfo.output_width * cinfo.output_components;
    /* Make a one-row-high sample array that will go away when done with image */
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    /* Step 6: while (scan lines remain to be read) */
    /*           jpeg_read_scanlines(...); */

    /* Here we use the library's state variable cinfo.output_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     */
    while(cinfo.output_scanline < cinfo.output_height){
        /* jpeg_read_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could ask for
         * more than one scanline at a time if that's more convenient.
         */
        jpeg_read_scanlines(&cinfo, buffer, 1);

        /* Assume put_scanline_someplace wants a pointer and sample count. */
        //put_scanline_someplace(buffer[0], row_stride);
    }

    /* Step 7: Finish decompression */

    jpeg_finish_decompress(&cinfo);
    /* We can ignore the return value since suspension is not possible
     * with the stdio data source.
     */

    /* Step 8: Release JPEG decompression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(&cinfo);

    /* At this point you may want to check to see whether any corrupt-data
     * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
     */

    /* And we're done! */
    return 0;
}

}
