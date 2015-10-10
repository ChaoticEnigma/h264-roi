#include "zwebp.h"

#include <webp/decode.h>
#include <webp/encode.h>

#define DEFAULT_QUALITY 100

namespace LibChaos {

ZWebP::ZWebP(ZImage *image) : _image(image){
    // Init decode config
    if(!WebPInitDecoderConfig(&decode_config))
        throw ZException("Failed to Init Decoder Config");

    // Init encode config
    if(!WebPConfigPreset(&encode_config, WEBP_PRESET_DEFAULT, DEFAULT_QUALITY))
        throw ZException("Failed to Preset Config");
    if(!WebPPictureInit(&picture))
        throw ZException("Failed to Init Picture");
}

bool ZWebP::isWebP(const ZBinary &data){
    WebPBitstreamFeatures test;
    return (WebPGetFeatures(data.raw(), data.size(), &test) == VP8_STATUS_OK);
    // First 12 bytes: R I F F * * * * W E B P
    //return (ZBinary::decbe32(data.raw()) == 0x52494646 && ZBinary::decbe32(data.raw() + 8) == 0x57454250);
}

bool ZWebP::decode(LibChaos::ZBinary &data, YImageBackend::ReadOptions *options){
    // Get webp file features
    VP8StatusCode status = WebPGetFeatures(data.raw(), data.size(), &decode_config.input);
    if(status != VP8_STATUS_OK)
        throw ZException("GetFeatures VP8 Status Error");

    const zu8 channels = (decode_config.input.has_alpha ? 4 : 3);
    // Size the image buffer
    _image->setDimensions((zu64)decode_config.input.width, (zu64)decode_config.input.height, channels, 8);
    // Allocate an uninitialized buffer
    _image->newData();

    // Configure decode output
    decode_config.output.colorspace = (decode_config.input.has_alpha ? MODE_RGBA : MODE_RGB);
    decode_config.output.is_external_memory = 1;
    decode_config.output.u.RGBA.rgba = _image->buffer();
    decode_config.output.u.RGBA.stride = _image->stride();
    decode_config.output.u.RGBA.size = _image->size();

    // Decode image
    status = WebPDecode(data.raw(), data.size(), &decode_config);
    if(status != VP8_STATUS_OK)
        throw ZException("Decode VP8 Status Error");

    // buffer is external, this call is safe
    WebPFreeDecBuffer(&decode_config.output);
    return true;
}

int MemoryWrite(const uint8_t* data, size_t data_size, const WebPPicture* picture){
    ZBinary *ptr = (ZBinary *)picture->custom_ptr;
    ptr->write(data, data_size);
    return 1;
}

bool ZWebP::encode(ZBinary &data, WriteOptions *options){
    // Encode config
    int conf_err = WebPValidateConfig(&encode_config);
    if(!conf_err)
        throw ZException("Invalid encode config");

    // Setup Picture
    picture.width = _image->width();
    picture.height = _image->height();
    if(!WebPPictureAlloc(&picture))
        throw ZException("Failed to alloc picture");

    // Setup writer
//    WebPMemoryWriter writer;
//    if(!WebPMemoryWriterInit(&writer))
//        throw ZException("Memory write init failed");
//    picture.writer = WebPMemoryWrite;
//    picture.custom_ptr = &writer;
    picture.writer = MemoryWrite;
    picture.custom_ptr = &data;

    // Encode image
    int ok = WebPEncode(&encode_config, &picture);
    WebPPictureFree(&picture);
    if(!ok)
        throw ZException("Encoding Error");

    return false;
}

}
