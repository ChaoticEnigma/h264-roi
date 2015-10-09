#include "zwebp.h"
#include "webp/decode.h"
#include "webp/encode.h"

namespace LibChaos {

bool ZWebP::isWebP(const ZBinary &data){
    // First 12 bytes: R I F F * * * * W E B P
    return (ZBinary::decbe32(data.raw()) == 0x52494646 && ZBinary::decbe32(data.raw() + 8) == 0x57454250);
}

bool ZWebP::decode(LibChaos::ZBinary &data, YImageBackend::ReadOptions *options){
    // Get webp file features
    WebPBitstreamFeatures features;
    VP8StatusCode status = WebPGetFeatures(data.raw(), data.size(), &features);
    if(status != VP8_STATUS_OK)
        throw ZException("VP8 Status Error");

    const zu8 channels = (features.has_alpha ? 4 : 3);
    // Size the image buffer
    _image->setDimensions((zu64)features.width, (zu64)features.height, channels, 8);
    // Allocate an uninitialized buffer
    _image->newData();

    // Decode image
    uint8_t *out;
    if(features.has_alpha){
        out = WebPDecodeRGBAInto(data.raw(), data.size(), _image->buffer(), _image->size(), features.width * channels);
    } else {
        out = WebPDecodeRGBInto(data.raw(), data.size(), _image->buffer(), _image->size(), features.width * channels);
    }
    if(out == NULL)
        throw ZException("WebP Decode Failed");

    return true;
}

bool ZWebP::encode(ZBinary &data, YImageBackend::WriteOptions *options){

    return false;
}

}
