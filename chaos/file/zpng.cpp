#include "zpng.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "zerror.h"
#include "zlog.h"
#include "zfile.h"

namespace LibChaos {

bool ZPNG::read(ZPath path){
    PngReadData *data = new PngReadData;

    try {
        ZFile file(path, ZFile::writeonly);
        if(!file.isOpen())
            throw ZError("PNG Read: cannot read file", PNGError::badfile, false);
        data->infile = file.fp();

        int result = readpng_init(data);
        switch(result){
        case 1:
            throw ZError("PNG Read: signature read fail", PNGError::sigreadfail, false);
        case 2:
            throw ZError("PNG Read: signature check fail", PNGError::sigcheckfail, false);
        case 3:
            throw ZError("PNG Read: read struct create fail", PNGError::readstructfail, false);
        case 4:
            throw ZError("PNG Read: info struct create fail", PNGError::infostructfail, false);
        case 5:
            throw ZError(ZString("PNG Read: ") + data->err_str, PNGError::libpngerror, false);
        default:
            break;
        }

        for(int i = 0; i < data->info_ptr->num_text; ++i){
            //LOG(data->info_ptr->text[i].key << ": " << data->info_ptr->text[i].text);
            text.push(data->info_ptr->text[i].key, data->info_ptr->text[i].text);
        }

        result = readpng_get_image(data, 1.0);
        switch(result){
        case 1:
            throw ZError(ZString("PNG Read: ") + data->err_str, PNGError::libpngerror, false);
            break;
        case 2:
            throw ZError("PNG Read: failed to alloc image data", PNGError::imageallocfail, false);
            break;
        case 3:
            throw ZError("PNG Read: failed to alloc row pointers", PNGError::rowallocfail, false);
            break;
        default:
            break;
        }

        if(!data->image_data){
            throw ZError("readpng_get_image failed", PNGError::badpointer, false);
        }

        image.setDimensions(data->width, data->height, data->channels, data->bit_depth);
        if(!image.validDimensions())
            throw ZError("PNG Read: invalid dimensions", PNGError::invaliddimensions, false);
        image.takeData(data->image_data);
        data->image_data = NULL;

//        if(data->channels == 3){
//            ZBitmapRGB tmp((PixelRGB*)data->image_data, data->width, data->height);
//            bitmap = tmp.recast<PixelRGBA>(0);
//        } else if(data->channels == 4){
//            bitmap = ZBitmapRGBA((PixelRGBA*)data->image_data, data->width, data->height);
//        } else {
//            throw ZError("Unsupported channel count", PNGError::unsupportedchannelcount, false);
//        }

    } catch(ZError e){
        error = e;
        ELOG("ZPNG Read error " << e.code() << ": " << e.what());
        readpng_cleanup(data);
        delete data;
        return false;
    }

    readpng_cleanup(data);
    delete data;
    return true;
}

bool ZPNG::write(ZPath path){
    PngWriteData *data = new PngWriteData;
    data->image_data = NULL;
    data->row_pointers = NULL;
    data->bit_depth = 8;
    data->have_bg = 0;
    data->have_time = 0;
    data->gamma = 0.0;
    data->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    data->width = image.width();
    data->height = image.height();

    data->interlaced = 1;

    try {
        if(!image.isLoaded())
            throw ZError("PNG Write: empty image", PNGError::emptyimage, false);

        //ZFile file(path, ZFile::writeonly);
        FILE *fp = fopen(path.str().cc(), "wb");
        //if(!file.isOpen())
        if(!fp)
            throw ZError("PNG Read: cannot write file", PNGError::badwritefile, false);
        //data->outfile = file.fp();
        data->outfile = fp;

        int result = writepng_init(data, text);
        switch(result){
        case 1:
            throw ZError("PNG Write: write struct create fail", PNGError::writestructfail, false);
        case 2:
            throw ZError("PNG Write: info struct create fail", PNGError::infostructfail, false);
        case 3:
            throw ZError(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
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
                throw ZError(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
            default:
                break;
            }
        } else {
            for(zu64 i = 0; i < image.height(); ++i){
                unsigned char *row = image.buffer() + (i * image.rowSize());
                result = writepng_encode_row(data, row);
                switch(result){
                case 1:
                    throw ZError(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
                default:
                    break;
                }
            }

            result = writepng_encode_finish(data);
            switch(result){
            case 1:
                throw ZError(ZString("PNG Write: ") + data->err_str, PNGError::libpngerror, false);
            default:
                break;
            }
        }
    } catch(ZError e){
        error = e;
        ELOG("ZPNG Write error " << e.code() << ": " << e.what());
        writepng_cleanup(data);
        delete data;
        return false;
    }

    writepng_cleanup(data);
    delete data;
    return true;
}

ZString ZPNG::libpngVersionInfo(){
    return ZString("Compiled libpng: ") << PNG_LIBPNG_VER_STRING << ", Using libpng: " << png_libpng_ver << ", Compiled zlib: " << ZLIB_VERSION << ", Using zlib: " << zlib_version;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////// Private
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ZPNG::readpng_init(PngReadData *data){
    // Check PNG signature
    unsigned char sig[8];
    if(fread(sig, 1, 8, data->infile) != 8){
        return 1;
    }
    if(png_sig_cmp(sig, 0, 1)){
        return 2;
    }

    // Create PNG read struct
    data->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, readpng_error_handler, readpng_warning_handler);
    if(!data->png_ptr){
        return 3;
    }

    // Create PNG info struct
    data->info_ptr = png_create_info_struct(data->png_ptr);
    if(!data->info_ptr){
        return 4;
    }

    // oh god somebody help
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 5;
    }

    // Read PNG info
    png_init_io(data->png_ptr, data->infile);
    png_set_sig_bytes(data->png_ptr, 8);
    png_read_info(data->png_ptr, data->info_ptr);

    // Retrieve PNG information
    png_get_IHDR(data->png_ptr, data->info_ptr, &data->width, &data->height, &data->bit_depth, &data->color_type, NULL, NULL, NULL);

    return 0;
}

int ZPNG::readpng_get_bgcolor(PngReadData *data){
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

int ZPNG::readpng_get_image(PngReadData *data, double display_exponent){
    // plz no not the face
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 1;
    }

    // Palette to RGB
    // <8-bit greyscale to 8-bit
    // transparency chunks to full alpha channel
    // strip 16-bit to 8-bit
    // grayscale to RGBA
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
        return 2;
    }

    // Alloc row pointers
    png_byte **row_pointers = new png_byte *[data->height];
    if(!row_pointers){
        return 3;
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

void ZPNG::readpng_cleanup(PngReadData *data){
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

void ZPNG::readpng_warning_handler(png_struct *png_ptr, png_const_charp msg){
    ELOG(ZLog::this_thread << "libpng read warning: " << msg);
}
void ZPNG::readpng_error_handler(png_struct *png_ptr, png_const_charp msg){
    //ELOG(ZLog::this_thread << "libpng read error: " << msg);
    PngReadData *data = (PngReadData *)png_get_error_ptr(png_ptr);
    if(data){
        data->err_str = ZString("libpng read error: ") + msg;
    }
    longjmp(png_jmpbuf(png_ptr), 1);
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ZPNG::writepng_init(PngWriteData *data, const AsArZ &texts){
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

    // Init I/O, outfile must be open in binary mode
    png_init_io(data->png_ptr, data->outfile);

    // Set the compression levels
    // we usually want to leave filtering turned on (except for palette images) and allow all of the filters (default)
    // we want 32K zlib window, unless entire image buffer is 16K or smaller (unknown here) (default)
    // we usually want max compression (NOT default)
    // the remaining compression flags should be left alone
    png_set_compression_level(data->png_ptr, Z_BEST_COMPRESSION);

    // this is default for no filtering; Z_FILTERED is default otherwise:
    //png_set_compression_strategy(data->png_ptr, Z_DEFAULT_STRATEGY);
    // these are all defaults:
    //png_set_compression_mem_level(data->png_ptr, 8);
    //png_set_compression_window_bits(data->png_ptr, 15);
    //png_set_compression_method(data->png_ptr, 8);

    // Select interlace type
    int interlace_type = data->interlaced ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

    // Set header data
    png_set_IHDR(data->png_ptr, data->info_ptr, data->width, data->height, data->bit_depth, data->color_type, interlace_type, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Set gamma
    if (data->gamma > 0.0)
        png_set_gAMA(data->png_ptr, data->info_ptr, data->gamma);

    // Set background
    if(data->have_bg){   /* we know it's RGBA, not gray+alpha */
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
        pngtext[i].key = (char *)texts.key(i).cc();
        pngtext[i].text = (char *)texts.val(i).cc();
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

int ZPNG::writepng_encode_image(PngWriteData *data){
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

int ZPNG::writepng_encode_row(PngWriteData *data, unsigned char *row_data){
    // not again
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 1;
    }

    // Write row pointed to by row_data
    png_write_row(data->png_ptr, row_data);

    return 0;
}

int ZPNG::writepng_encode_finish(PngWriteData *data){
    // no more please
    if(setjmp(png_jmpbuf(data->png_ptr))){
        return 1;
    }

    // End write
    png_write_end(data->png_ptr, NULL);

    return 0;
}

void ZPNG::writepng_cleanup(PngWriteData *mainprog_ptr){
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

void ZPNG::writepng_warning_handler(png_struct *png_ptr, png_const_charp msg){
    ELOG(ZLog::this_thread << "libpng write warning: " << msg);
}
void ZPNG::writepng_error_handler(png_struct *png_ptr, png_const_charp msg){
    //ELOG(ZLog::this_thread << "libpng write error: " << msg);
    PngWriteData *data = (PngWriteData *)png_get_error_ptr(png_ptr);
    if(data){
        data->err_str = ZString("libpng write error: ") + msg;
    }
    longjmp(png_jmpbuf(png_ptr), 1);
}

}
