/*******************************************************************************
**                                  LibChaos                                  **
**                                   zpng.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZPNG_H
#define ZPNG_H

#include "ztypes.h"
#include "zimage.h"
#include "zstring.h"
#include "zexception.h"
#include "zbinary.h"
#include "yimagebackend.h"

//#define PNG_DEBUG 3
#include <png.h>

namespace LibChaos {

class ZPNG : public YImageBackend {
public:
    struct PNGError {
        enum pngerrors {
            none = 0,
            badfile = 1,
            sigreadfail = 2,
            sigcheckfail = 3,
            readstructfail = 4,
            infostructfail = 5,
            libpngerror = 6,
            imageallocfail = 7,
            rowallocfail = 8,
            badpointer = 9,
            unsupportedchannelcount = 10,
            emptyimage = 11,
            writestructfail = 12,
            badwritefile = 13,
            invaliddimensions = 14,
            invalidcolortype = 15,
            invalidbkgddepth = 16,
            emptyinput = 17,
        };
    };

    struct PngChunk {
        zu32 size;
        ZString name;
        ZBinary data;
        zu32 crc;
        bool crc_ok;
    };

    struct PngInfo {
        PngInfo() : interlaced(false), gamma(0.0){}

        bool interlaced;
        double gamma;
    };

public:
    ZPNG(ZImage *image);

    static bool isPNG(const ZBinary &data);

    /*! Decode a PNG image.
     *  \throws ZException On decoding errors.
     */
    bool decode(ZReader *input);
    bool encode(ZWriter *output);

    static ZArray<PngChunk> parsePngChunks(ZBinary pngdata);
    static ZArray<PngChunk> parsePngAncillaryChunks(ZBinary pngdata);

    static ZString libpngVersionInfo();

    ZString &pngText(ZString key){
        return text[key];
    }

public:
    //! Strip 16-bit colors to 8-bit colors on decoding.
    bool strip = false;
    //! Interlace PNG on encoding.
    bool interlace = false;
    //! Text Chunks to write on encoding.
    AsArZ text;

private:
    ZImage *_image;
};

}

#endif // ZPNG_H
