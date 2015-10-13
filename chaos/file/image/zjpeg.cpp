/*******************************************************************************
**                                  LibChaos                                  **
**                                  zjpeg.cpp                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zjpeg.h"
#include "zfile.h"
#include "zexception.h"
#include "zlog.h"

#include <stdio.h>
#include <jpeglib.h>

#define JPEG_SIG 0xFFD8FFE0

namespace LibChaos {

int readjpeg(ZReader *in, ZImage *out);

ZJPEG::ZJPEG(ZImage *image) : _image(image){
    quality = 100;
}

bool ZJPEG::isJPEG(const ZBinary &data){
    return (ZBinary::decbe32(data.raw()) == JPEG_SIG);
}

bool ZJPEG::decode(ZReader *input){
    readjpeg(input, _image);
    return false;
}

bool ZJPEG::encode(ZWriter *output){

    return false;
}

// Custom error struct
typedef struct my_error_mgr {
    struct jpeg_error_mgr pub; // "public" fields
} *my_error_ptr;

// Custom error handler
METHODDEF(void) my_error_exit(j_common_ptr cinfo){
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

    // Display the message
    (*cinfo->err->output_message) (cinfo);

    // Throw exception
    throw ZException("JPEG Error");
}

// JPEG Interface from libjpeg's example.c

int readjpeg(ZReader *in, ZImage *out){
    // Decompression struct
    struct jpeg_decompress_struct cinfo;

    // Setup custom error handling
    struct my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    // Initialize decompression struct
    jpeg_create_decompress(&cinfo);

    ZBinary data(in->available());
    if(in->read(data.raw(), data.size()) != data.size())
        throw ZException("Failed to read expected data");

    // Setup JPEG data source from memory
    jpeg_mem_src(&cinfo, data.raw(), data.size());

    // Read JPEG header
    boolean status = jpeg_read_header(&cinfo, TRUE);

    // Start decoding
    status = jpeg_start_decompress(&cinfo);

    // Only RGB output colorspace is supported
    if(cinfo.out_color_space != JCS_RGB){
        throw ZException("JPEG not RGB");
    }

    // Allocate image buffer
    out->setDimensions(cinfo.output_width, cinfo.output_height, cinfo.output_components, 8);
    out->newData();

    // Create sample buffer
    const unsigned int buffer_count = 1;
    // The buffer is an array of pointers to sample rows
    JSAMPARRAY buffer = new JSAMPROW[buffer_count];
    // Each row is an array of samples
    //buffer[0] = new JSAMPLE[out->stride()];

    // Read image scanlines
    while(cinfo.output_scanline < cinfo.output_height){
        // Set pointer to next output row
        buffer[0] = out->buffer() + ((cinfo.output_scanline - 1) * out->stride());
        // Decode scanline(s) into buffer
        jpeg_read_scanlines(&cinfo, buffer, buffer_count);
        // Copy scanline to buffer
        //memcpy(out->buffer() + ((cinfo.output_scanline - 1) * out->stride()), buffer[0], out->stride());
    }

    // Cleanup
    //delete[] buffer[0];
    delete[] buffer;

    status = jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    // Check for decoding warnings
    if(jerr.pub.num_warnings > 0){
        ELOG("JPEG Warnings: " << jerr.pub.num_warnings);
    }

    return 0;
}

}
