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

void error_exit(j_common_ptr cinfo);

ZJPEG::ZJPEG(ZImage *image) : _image(image){
    quality = 100;
    // Setup custom error handling
    cinfo.err = jpeg_std_error(&jpeg_error);
    jpeg_error.error_exit = error_exit;
    // Initialize decompression struct
    jpeg_create_decompress(&cinfo);
}

ZJPEG::~ZJPEG(){
    jpeg_destroy_decompress(&cinfo);
}

bool ZJPEG::isJPEG(const ZBinary &data){
    try {
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jerr.error_exit = error_exit;
        jpeg_create_decompress(&cinfo);
        jpeg_mem_src(&cinfo, data.raw(), data.size());
        int status = jpeg_read_header(&cinfo, TRUE);
        jpeg_destroy_decompress(&cinfo);
        return (status == JPEG_HEADER_OK);
    } catch(ZException e){
        ELOG("ZJPEG::isJPEG(): " << e.what());
        return false;
    }
}

// JPEG Interface from libjpeg's example.c
bool ZJPEG::decode(ZReader *input){
    try {
        // Copy input
        ZBinary data(input->available());
        if(input->read(data.raw(), data.size()) != data.size()){
            throw ZException("Failed to read expected data");
        }

        // Setup JPEG data source from memory
        jpeg_mem_src(&cinfo, data.raw(), data.size());

        // Read JPEG header
        int ret = jpeg_read_header(&cinfo, TRUE);
        if(ret != JPEG_HEADER_OK){
            throw ZException("Failed to read JPEG header");
        }

        // Start decoding
        boolean status = jpeg_start_decompress(&cinfo);

        // Only RGB output colorspace is supported
        if(cinfo.out_color_space != JCS_RGB){
            throw ZException("JPEG is not RGB");
        }

        DLOG(cinfo.output_width << " " << cinfo.output_height << " " << cinfo.output_components);

        // Allocate image buffer
        _image->setDimensions(cinfo.output_width, cinfo.output_height, cinfo.output_components, 8);
        _image->newData();

        // Create sample buffer
        const unsigned int buffer_count = 1;
        // The buffer is an array of pointers to sample rows
        JSAMPARRAY buffer = new JSAMPROW[buffer_count];
        // Each row is an array of samples
        buffer[0] = new JSAMPLE[_image->stride()];

        // Read image scanlines
        while(cinfo.output_scanline < cinfo.output_height){
            // Set pointer to next output row
            //buffer[0] = _image->buffer() + ((cinfo.output_scanline - 1) * _image->stride());
            // Decode scanline(s) into buffer
            jpeg_read_scanlines(&cinfo, buffer, buffer_count);
            // Copy scanline to buffer
            memcpy(_image->buffer() + ((cinfo.output_scanline - 1) * _image->stride()), buffer[0], _image->stride());
        }

        // Cleanup
        delete[] buffer[0];
        delete[] buffer;

        status = jpeg_finish_decompress(&cinfo);

        // Check for decoding warnings
        if(jpeg_error.num_warnings > 0){
            ELOG("JPEG Warnings: " << jpeg_error.num_warnings);
        }

    } catch(ZException e){
        ELOG("ZJPEG::decode(): " << e.what());
        return false;
    }
    return true;
}

bool ZJPEG::encode(ZWriter *output){

    return false;
}

void error_exit(j_common_ptr cinfo){
    // Display the message
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    // Throw exception
    throw ZException(ZString("JPEG Error: ") + buffer);
}

}
