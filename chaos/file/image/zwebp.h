#ifndef ZWEBP_H
#define ZWEBP_H

#include "zimage.h"
#include "yimagebackend.h"

#include <webp/decode.h>
#include <webp/encode.h>

namespace LibChaos {

//! WebP encoder/decoder, libwebp wrapper.
class ZWebP : public YImageBackend {
public:
    ZWebP(ZImage *image);

    static bool isWebP(const ZBinary &data);

    /*! Decode a WebP image.
     *  Can decode 8-bit RGB and RGBA WebP images.
     *  \throws ZException On decoding errors.
     */
    bool decode(ZReader *input);
    bool encode(ZWriter *output);

public:
    // Decoding
    WebPDecoderConfig decode_config;

    // Encoding
    WebPConfig encode_config;
    WebPPicture picture;

private:
    ZImage *_image;
};

}

#endif // ZWEBP_H
