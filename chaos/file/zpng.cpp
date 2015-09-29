/*******************************************************************************
**                                  LibChaos                                  **
**                                  zpng.cpp                                  **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zpng.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "zexception.h"
#include "zlog.h"
#include "zfile.h"

#include <png.h>
#include <zlib.h>

using namespace LibChaos;

struct PngReadData {
    ZBinary *filedata;

    png_struct *png_ptr = NULL;
    png_info *info_ptr = NULL;

    png_uint_32 width, height;
    unsigned char channels, fchannels;
    int bit_depth, fbit_depth, color_type;
    unsigned char bg_red, bg_green, bg_blue;
    double gamma;

    unsigned char *image_data = nullptr;

    ZString err_str;
};

struct PngWriteData {
    ZBinary *filedata;

    png_struct *png_ptr = NULL;
    png_info *info_ptr = NULL;

    png_uint_32 width, height;
    int bit_depth, color_type;

    unsigned char *image_data = NULL;
    unsigned char **row_pointers = NULL;

    double gamma;

    bool interlaced;

    bool have_bg;
    unsigned char bg_red, bg_green, bg_blue;

    bool have_time;
    time_t modtime;

    ZString err_str;
};

int readpng_init(PngReadData *data);
void readpng_read_fn(png_struct *png_ptr, png_byte *outbytes, png_size_t bytestoread);
int readpng_get_bgcolor(PngReadData *data);
int readpng_get_image(PngReadData *data, double display_exponent);
void readpng_cleanup(PngReadData *data);
void readpng_warning_handler(png_struct *png_ptr, png_const_charp msg);
void readpng_error_handler(png_struct *png_ptr, png_const_charp msg);

int writepng_init(PngWriteData *mainprog_ptr, const AsArZ &text);
void writepng_write_fn(png_struct *png_ptr, png_byte *inbytes, png_size_t length);
int writepng_encode_image(PngWriteData *mainprog_ptr);
int writepng_encode_row(PngWriteData *mainprog_ptr, unsigned char *row);
int writepng_encode_finish(PngWriteData *mainprog_ptr);
void writepng_cleanup(PngWriteData *mainprog_ptr);
void writepng_warning_handler(png_struct *png_ptr, png_const_charp msg);
void writepng_error_handler(png_struct *png_ptr, png_const_charp msg);

namespace LibChaos {

bool ZPNG::decode(ZBinary &pngdata_in){
    PngReadData data;

    try {
        if(!pngdata_in.size()){
            throw ZException("PNG Read: empty input", PNGError::emptyinput, false);
        }

        data.filedata = &pngdata_in;

        readpng_init(&data);

        // Get text sections
        //for(int i = 0; i < data.info_ptr->num_text; ++i){
        //    text.push(data.info_ptr->text[i].key, data.info_ptr->text[i].text);
        //}

//        data->bg_red = 0;
//        data->bg_green = 0;
//        data->bg_blue = 0;
//        result = readpng_get_bgcolor(data);
//        switch(result){
//        case 1:
//            throw ZError(ZString("PNG Read: ") + data->err_str, PNGError::libpngerror, false);
//        case 2:
//            // No background chunk, ignore
//            break;
//        case 3:
//            throw ZError("PNG Read: invalid bkgd color depth", PNGError::invalidbkgddepth, false);
//        default:
//            //png_set_background(data->png_ptr, );
//            break;
//        }

        double default_display_exponent = 1.0;
        readpng_get_image(&data, default_display_exponent);

        if(!data.image_data){
            throw ZException("readpng_get_image failed", PNGError::badpointer, false);
        }

        image.setDimensions(data.width, data.height, data.channels, data.bit_depth);
        if(!image.validDimensions())
            throw ZException(ZString("PNG Read: invalid dimensions ") + image.width() + "x" + image.height() + " " + image.channels() + "," + image.depth() + " " + data.color_type, PNGError::invaliddimensions, false);
        image.takeData(data.image_data);
        data.image_data = NULL;

    } catch(ZException e){
        //ELOG("ZPNG Read error " << e.code() << ": " << e.what());
        pngdata_in.rewind();
        readpng_cleanup(&data);
        throw e;
    }

    pngdata_in.rewind();
    readpng_cleanup(&data);
    return true;
}

bool ZPNG::encode(ZBinary &pngdata_out, void *options){
    PNGWrite::pngoptions pngoptions = *(PNGWrite::pngoptions*)options;
    PngWriteData *data = new PngWriteData;

    try {
        if(!image.isLoaded())
            throw ZException("PNG Write: empty image", PNGError::emptyimage, false);

        data->filedata = &pngdata_out;

        data->image_data = NULL;
        data->row_pointers = NULL;
        data->have_time = false;
        data->gamma = 0.0;

        data->have_bg = false;
        data->bg_red = 255;
        data->bg_green = 0;
        data->bg_blue = 0;

        data->bit_depth = image.depth();
        data->width = image.width();
        data->height = image.height();

        if(image.channels() == 1)
            data->color_type = PNG_COLOR_TYPE_GRAY;
        else if(image.channels() == 2)
            data->color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
        else if(image.channels() == 3)
            data->color_type = PNG_COLOR_TYPE_RGB;
        else if(image.channels() == 4)
            data->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        else
            throw ZException("PNG Read: unsupported image channel count", PNGError::unsupportedchannelcount, false);

        data->interlaced = pngoptions & PNGWrite::interlace;

        int result = writepng_init(data, text);
        switch(result){
        case 1:
            throw ZException("PNG Write: write struct create fail", PNGError::writestructfail, false);
        case 2:
            throw ZException("PNG Write: info struct create fail", PNGError::infostructfail, false);
        case 3:
            throw ZException(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
        default:
            break;
        }

        if(data->interlaced){
            data->row_pointers = new unsigned char*[image.height() * sizeof(unsigned char *)];
            for(zu64 i = 0; i < image.height(); ++i){
                data->row_pointers[i] = image.buffer() + (i * image.rowSize());
            }

            result = writepng_encode_image(data);
            switch(result){
            case 1:
                throw ZException(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
            default:
                break;
            }
        } else {
            for(zu64 i = 0; i < image.height(); ++i){
                unsigned char *row = image.buffer() + (i * image.rowSize());
                result = writepng_encode_row(data, row);
                switch(result){
                case 1:
                    throw ZException(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
                default:
                    break;
                }
            }

            result = writepng_encode_finish(data);
            switch(result){
            case 1:
                throw ZException(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
            default:
                break;
            }
        }

    } catch(ZException e){
        //ELOG("ZPNG Write error " << e.code() << ": " << e.what());
        writepng_cleanup(data);
        delete data;
        throw e;
    }

    writepng_cleanup(data);
    delete data;
    return true;
}

bool ZPNG::read(ZPath path){
    ZBinary data;
    ZFile::readBinary(path, data);

    if(!data.size()){
        throw ZException("PNG Read: empty read file", PNGError::badfile, false);
    }
    return decode(data);
}

bool ZPNG::write(ZPath path, PNGWrite::pngoptions options){
    ZBinary data;
    if(!encode(data, options))
        return false;

    if(!ZFile::writeBinary(path, data)){
        throw ZException("PNG Read: cannot write file", PNGError::badwritefile, false);
    }
    return true;
}

ZArray<ZPNG::PngChunk> ZPNG::parsePngChunks(ZBinary pngdata){
    ZArray<PngChunk> chunks;
    zu64 size = 0;
    unsigned char *buffer = nullptr;

    pngdata.read(nullptr, 8);

    while(!pngdata.atEnd()){
        PngChunk chunk;

        // Size (big-endian in PNG chunks)
        chunk.size = pngdata.readzu32();

        // Name
        size = 4;
        buffer = new unsigned char[size];
        pngdata.read(buffer, size);
        chunk.name = ZString((const char *)buffer, size);
        delete[] buffer;

        // Data
        size = chunk.size;
        buffer = new unsigned char[size];
        pngdata.read(buffer, size);
        chunk.data = ZBinary(buffer, size);
        delete[] buffer;

        // CRC
        chunk.crc = pngdata.readzu32();
        chunk.crc_ok = (chunk.crc == ZHash32Base::crcHash32_hash(chunk.data.raw(), chunk.size));

        chunks.push(chunk);
    }

    return chunks;
}


ZArray<ZPNG::PngChunk> ZPNG::parsePngAncillaryChunks(ZBinary pngdata){
    ZArray<PngChunk> chunks = parsePngChunks(pngdata);
    for(zu64 i = chunks.size(); i > 0; --i){
        if(chunks[i-1].name == "IHDR" ||
           chunks[i-1].name == "IDAT" ||
           chunks[i-1].name == "IEND"){
            chunks.erase(i-1);
        }
    }
    return chunks;
}

ZString ZPNG::libpngVersionInfo(){
    return ZString("Compiled with libpng ") << PNG_LIBPNG_VER_STRING << ", Using libpng " << png_libpng_ver << ", Compiled with zlib " << ZLIB_VERSION << ", Using zlib " << zlib_version;
}

}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////// Private
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int readpng_init(PngReadData *data){
    // Initialize png image struct
    //memset(&data->image, 0, (sizeof data->image));
    //data->image.version = PNG_IMAGE_VERSION;

    // Check PNG signature
    if(png_sig_cmp(data->filedata->raw(), 0, 1)){
        throw ZException("PNG Read: signature check fail", ZPNG::PNGError::sigcheckfail, false);
        return 2;
    }
    data->filedata->setPos(8);

    // Create PNG read struct
    data->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, data, readpng_error_handler, readpng_warning_handler);
    if(data->png_ptr == NULL){
        throw ZException("PNG Read: read struct create fail", ZPNG::PNGError::readstructfail, false);
        return 3;
    }

    // Create PNG info struct
    data->info_ptr = png_create_info_struct(data->png_ptr);
    if(data->info_ptr == NULL){
        throw ZException("PNG Read: info struct create fail", ZPNG::PNGError::infostructfail, false);
        return 4;
    }

    // oh god somebody help
    //if(setjmp(png_jmpbuf(data->png_ptr))){
    //    throw ZException(ZString("PNG Read: ") + data.err_str, ZPNG::PNGError::libpngerror, false);
    //    return 5;
    //}

    // Set custom reading function
    png_set_read_fn(data->png_ptr, (void *)data->filedata, readpng_read_fn);

    // Already read the signature
    png_set_sig_bytes(data->png_ptr, 8);

#if 0
    png_read_png(data->png_ptr, data->info_ptr, png_transforms, NULL);
#else
    // Read PNG info
    png_read_info(data->png_ptr, data->info_ptr);
    // Retrieve PNG information
    png_get_IHDR(data->png_ptr, data->info_ptr, &data->width, &data->height, &data->bit_depth, &data->color_type, NULL, NULL, NULL);
#endif

    return 0;
}

void readpng_read_fn(png_struct *png_ptr, png_byte *outbytes, png_size_t bytestoread){
    // Check user pointer
    void *ioptr = png_get_io_ptr(png_ptr);
    if(ioptr == NULL){
        throw ZException("read fn null io ptr");
        //longjmp(png_jmpbuf(png_ptr), 1);
    }

    ZBinary *filedata = (ZBinary*)ioptr;
    filedata->read(outbytes, bytestoread);
}

int readpng_get_bgcolor(PngReadData *data){
    // i don't have a choice...
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 1;
    }

    // Check that bKGD chuck is valid
    if(!png_get_valid(data->png_ptr, data->info_ptr, PNG_INFO_bKGD)){
        return 2;
    }

    // Get background color
    png_color_16 *background;
    png_get_bKGD(data->png_ptr, data->info_ptr, &background);

    // Adjust background colors for bit depth
    // Expand to at least 8 bits
    if(data->bit_depth == 16){
        data->bg_red   = background->red   >> 8;
        data->bg_green = background->green >> 8;
        data->bg_blue  = background->blue  >> 8;
    } else if(data->color_type == PNG_COLOR_TYPE_GRAY && data->bit_depth < 8){
        if(data->bit_depth == 1)
            data->bg_red = data->bg_green = data->bg_blue = background->gray ? 255 : 0;
        else if(data->bit_depth == 2)
            data->bg_red = data->bg_green = data->bg_blue = background->gray * (255 / 3);
        else if(data->bit_depth == 4)
            data->bg_red = data->bg_green = data->bg_blue = background->gray * (255 / 15);
        else
            return 3;
    } else {
        data->bg_red   = (unsigned char)background->red;
        data->bg_green = (unsigned char)background->green;
        data->bg_blue  = (unsigned char)background->blue;
    }

    return 0;
}

int readpng_get_image(PngReadData *data, double display_exponent){
    // plz no not the face
    //if(setjmp(png_jmpbuf(data->png_ptr))){
    //    throw ZException(ZString("PNG Read: ") + data.err_str, ZPNG::PNGError::libpngerror, false);
    //    return 1;
    //}

//    // Transform transparency chunks to full alpha channel
//    if(png_get_valid(data->png_ptr, data->info_ptr, PNG_INFO_tRNS)){
//        png_set_tRNS_to_alpha(data->png_ptr);
//    }

    // Strip 16-bit to 8-bit
    png_set_strip_16(data->png_ptr);

    bool needbg = false;

    // Do some transformations based on the color type
    // We ultimately want 8 or 16-bit RGB / RGBA
    switch(data->color_type){
    case PNG_COLOR_TYPE_PALETTE:
        png_set_palette_to_rgb(data->png_ptr);
        if(png_get_valid(data->png_ptr, data->info_ptr, PNG_INFO_tRNS)){
            png_set_tRNS_to_alpha(data->png_ptr);
            needbg = true;
        } else {
            png_set_filler(data->png_ptr, 0xFF, PNG_FILLER_AFTER);
        }
        break;

    case PNG_COLOR_TYPE_GRAY:
        png_set_expand_gray_1_2_4_to_8(data->png_ptr);
        png_set_gray_to_rgb(data->png_ptr);
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        png_set_gray_to_rgb(data->png_ptr);
        break;

    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        // No change
        break;
    default:
        throw ZException("PNG Read: invalid color type", ZPNG::PNGError::invalidcolortype, false);
        return 2;
    }

    png_color_16 *image_background;
    if(png_get_bKGD(data->png_ptr, data->info_ptr, &image_background)){
        // set background color
        png_set_background(data->png_ptr, image_background, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    } else if(needbg){
        // fallback white background
        png_color_16 my_background;
        my_background.red = 255;
        my_background.green = 255;
        my_background.blue = 255;
        png_set_background(data->png_ptr, &my_background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
    }

    // Set gamma if file has gamma
    double gamma;
    if(png_get_gAMA(data->png_ptr, data->info_ptr, &gamma))
        png_set_gamma(data->png_ptr, display_exponent, gamma);

    // Update PNG info data
    png_read_update_info(data->png_ptr, data->info_ptr);

    // Get rowbytes and channels
    png_uint_32 rowbytes = png_get_rowbytes(data->png_ptr, data->info_ptr);
    data->channels = png_get_channels(data->png_ptr, data->info_ptr);

    // Alloc image data
    data->image_data = new unsigned char[rowbytes * data->height];
    if(!data->image_data){
        throw ZException("PNG Read: failed to alloc image data", ZPNG::PNGError::imageallocfail, false);
        return 3;
    }

    // Alloc row pointers
    png_byte **row_pointers = new png_byte *[data->height];
    if(!row_pointers){
        throw ZException("PNG Read: failed to alloc row pointers", ZPNG::PNGError::rowallocfail, false);
        return 4;
    }

    // Point row pointers at image data
    for(zu64 i = 0; i < data->height; ++i){
        row_pointers[i] = data->image_data + (i * rowbytes);
    }

    // Read the image
    png_read_image(data->png_ptr, row_pointers);

    delete[] row_pointers;

    // End read
    png_read_end(data->png_ptr, NULL);

    return 0;
}

void readpng_cleanup(PngReadData *data){
    if(data->image_data){
        delete[] data->image_data;
        data->image_data = nullptr;
    }

    if(data->png_ptr){
        if(data->info_ptr){
            png_destroy_read_struct(&data->png_ptr, &data->info_ptr, NULL);
            data->info_ptr = NULL;
        } else {
            png_destroy_read_struct(&data->png_ptr, NULL, NULL);
        }
        data->png_ptr = NULL;
    }
}

void readpng_warning_handler(png_struct *png_ptr, png_const_charp msg){
    ELOG(ZLog::this_thread << "libpng read warning: " << msg);
}
void readpng_error_handler(png_struct *png_ptr, png_const_charp msg){
    //ELOG(ZLog::this_thread << "libpng read error: " << msg);
    PngReadData *data = (PngReadData *)png_get_error_ptr(png_ptr);
    if(data){
        data->err_str = ZString("libpng read error: ") + msg;
    }
    throw ZException(ZString("libpng read error: ") + msg);
    //longjmp(png_jmpbuf(png_ptr), 1);
}

// /////////////////////////////////////////////////////////////////////////////
// // Write ////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////

int writepng_init(PngWriteData *data, const AsArZ &texts){
    // Create write struct
    data->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, data, writepng_error_handler, writepng_warning_handler);
    if(!data->png_ptr){
        return 1;
    }

    // Create info struct
    data->info_ptr = png_create_info_struct(data->png_ptr);
    if(!data->info_ptr){
        return 2;
    }

    // damnit
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 3;
    }

    // Set custom write function
    png_set_write_fn(data->png_ptr, data->filedata, writepng_write_fn, NULL);

    // Set filter heuristics
    //png_set_filter_heuristics(data->png_ptr, PNG_FILTER_HEURISTIC_UNWEIGHTED, 0, NULL, NULL);
    png_set_filter_heuristics(data->png_ptr, PNG_FILTER_HEURISTIC_WEIGHTED, 0, NULL, NULL);

    // Enable all filters
    png_set_filter(data->png_ptr, 0, PNG_ALL_FILTERS);
    // Filtering should be disabled on palette images
    //png_set_filter(data->png_ptr, 0, PNG_FILTER_NONE);
    //png_set_filter(data->png_ptr, 0, PNG_FILTER_SUB);
    //png_set_filter(data->png_ptr, 0, PNG_FILTER_UP);
    //png_set_filter(data->png_ptr, 0, PNG_FILTER_AVG);
    //png_set_filter(data->png_ptr, 0, PNG_FILTER_PAETH);

    // We want max compression
    png_set_compression_level(data->png_ptr, Z_BEST_COMPRESSION);

    png_set_compression_strategy(data->png_ptr, Z_DEFAULT_STRATEGY);
    //png_set_compression_strategy(data->png_ptr, Z_FILTERED);

    // Must be Z_DEFLATED
    png_set_compression_method(data->png_ptr, Z_DEFLATED);

    // Maximum window size
    png_set_compression_window_bits(data->png_ptr, 15);
    // Maximum memory usage
    png_set_compression_mem_level(data->png_ptr, 9);
    // Compression buffer size
    png_set_compression_buffer_size(data->png_ptr, 8192);

    // Select interlace type
    int interlace_type = data->interlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    // Set header data
    png_set_IHDR(data->png_ptr, data->info_ptr, data->width, data->height, data->bit_depth, data->color_type, interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Set gamma
    if(data->gamma > 0.0)
        png_set_gAMA(data->png_ptr, data->info_ptr, data->gamma);

    // Set background
    if(data->have_bg){
        png_color_16 background;
        background.red = data->bg_red;
        background.green = data->bg_green;
        background.blue = data->bg_blue;
        png_set_bKGD(data->png_ptr, data->info_ptr, &background);
    }

    // Set modified time
    if (data->have_time) {
        png_time  modtime;
        png_convert_from_time_t(&modtime, data->modtime);
        png_set_tIME(data->png_ptr, data->info_ptr, &modtime);
    }

    // Add text to PNG
    png_text *pngtext = new png_text[texts.size()];
    for(zu64 i = 0; i < texts.size(); ++i){
        pngtext[i].compression = PNG_TEXT_COMPRESSION_NONE;
        pngtext[i].key = texts.key(i).c();
        pngtext[i].text = texts.val(i).c();
    }
    png_set_text(data->png_ptr, data->info_ptr, pngtext, (int)texts.size());
    delete[] pngtext;

    // Write info chunks
    png_write_info(data->png_ptr, data->info_ptr);

    // set up the transformations
    // just pack low-bit-depth pixels into bytes (one, two or four pixels per byte)
    png_set_packing(data->png_ptr);
    //png_set_shift(png_ptr, &sig_bit);  // to scale low-bit-depth values

    return 0;
}

void writepng_write_fn(png_struct *png_ptr, png_byte *inbytes, png_size_t length){
    // Check user pointer
    void *ioptr = png_get_io_ptr(png_ptr);
    if(ioptr == NULL){
        throw ZException("write fn null io ptr");
        //longjmp(png_jmpbuf(png_ptr), 1);
    }

    ZBinary *filedata = (ZBinary *)ioptr;
    filedata->concat(ZBinary(inbytes, length));
}

int writepng_encode_image(PngWriteData *data){
    // *sigh*
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 1;
    }

    // Write the whole image
    png_write_image(data->png_ptr, data->row_pointers);

    // End writing
    png_write_end(data->png_ptr, NULL);

    return 0;
}

int writepng_encode_row(PngWriteData *data, unsigned char *row_data){
    // not again
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 1;
    }

    // Write row pointed to by row_data
    png_write_row(data->png_ptr, row_data);

    return 0;
}

int writepng_encode_finish(PngWriteData *data){
    // no more please
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 1;
    }

    // End write
    png_write_end(data->png_ptr, NULL);

    return 0;
}

void writepng_cleanup(PngWriteData *mainprog_ptr){
    if(mainprog_ptr->png_ptr){
        if(mainprog_ptr->info_ptr){
            png_destroy_write_struct(&mainprog_ptr->png_ptr, &mainprog_ptr->info_ptr);
            mainprog_ptr->info_ptr = NULL;
        } else {
            png_destroy_write_struct(&mainprog_ptr->png_ptr, NULL);
        }
        mainprog_ptr->png_ptr = NULL;
    }
}

void writepng_warning_handler(png_struct *png_ptr, png_const_charp msg){
    ELOG(ZLog::this_thread << "libpng write warning: " << msg);
}
void writepng_error_handler(png_struct *png_ptr, png_const_charp msg){
    //ELOG(ZLog::this_thread << "libpng write error: " << msg);
    PngWriteData *data = (PngWriteData *)png_get_error_ptr(png_ptr);
    if(data){
        data->err_str = ZString("libpng write error: ") + msg;
    }
    longjmp(png_jmpbuf(png_ptr), 1);
}
