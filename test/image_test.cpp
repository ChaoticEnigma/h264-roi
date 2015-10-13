#include "test.h"
#include "zimage.h"
#include "zfile.h"

#define IMAGE_TEST_BMP        "image_test.bmp"
#define IMAGE_TEST_PPM        "image_test.ppm"
#define IMAGE_TEST_PNG        "image_test.png"
#define IMAGE_TEST_INVERT_PNG "image_test_invert.png"
#define IMAGE_TEST_16_PNG     "image_test_16.png"

int image_test(){

    // 8-bit

    zu64 width = 1920;
    zu64 height = 1080;
    LOG("Creating " << width << "x" << height << " 8-bit color image");
    ZImage image1(width, height, ZImage::RGB24);
    image1.newData();

    for(zu64 i = 0; i < image1.pixels(); ++i){
        image1.pixelAt(i)[0] = 0x33;
        image1.pixelAt(i)[1] = 0xCC;
        image1.pixelAt(i)[2] = 0xFF;
    }

    zu64 o = (width - (height+200)) / 2;
    for(zu64 y = 0; y < image1.height(); y++){
        for(zu64 x = 0; x < 200; ++x){
            image1.pixelAt(x+o, y)[0] = 0xFF;
            image1.pixelAt(x+o, y)[1] = 0xAA;
            image1.pixelAt(x+o, y)[2] = 0x00;
        }
        ++o;
    }

    ZBinary out1;
    image1.setFormat(ZImage::BMP);
    image1.encodeFormat(out1);
    TASSERT(ZFile::writeBinary(IMAGE_TEST_BMP, out1));

    out1.clear();
    image1.setFormat(ZImage::PPM);
    image1.encodeFormat(out1);
    TASSERT(ZFile::writeBinary(IMAGE_TEST_PPM, out1));

    out1.clear();
    image1.setFormat(ZImage::PNG);
    image1.encodeFormat(out1);
    TASSERT(ZFile::writeBinary(IMAGE_TEST_PNG, out1));

    ZBinary in1;
    TASSERT(ZFile::readBinary(IMAGE_TEST_PNG, in1));

    ZImage imagein1(in1);
    LOG("Invert image: " << (int)imagein1.getFormat() << " " << imagein1.width() << "x" << imagein1.height());
    for(zu64 i = 0; i < imagein1.pixels(); ++i){
        imagein1.pixelAt(i)[0] = 0xFF - imagein1.pixelAt(i)[0];
        imagein1.pixelAt(i)[1] = 0xFF - imagein1.pixelAt(i)[1];
        imagein1.pixelAt(i)[2] = 0xFF - imagein1.pixelAt(i)[2];
    }

    out1.clear();
    image1.encodeFormat(out1);
    TASSERT(ZFile::writeBinary(IMAGE_TEST_INVERT_PNG, out1));

    // 16-bit

    LOG("Creating " << width << "x" << height << " 16-bit color image");
    ZImage image2(width, height, ZImage::RGB48);
    image2.newData();

    for(zu64 i = 0; i < image2.pixels(); ++i){
        ((zu16 *)image2.pixelAt(i))[0] = 0x3333;
        ((zu16 *)image2.pixelAt(i))[1] = 0xCCCC;
        ((zu16 *)image2.pixelAt(i))[2] = 0xFFFF;
    }

    o = (width - (height+200)) / 2;
    for(zu64 y = 0; y < image2.height(); y++){
        for(zu64 x = 0; x < 200; ++x){
            ((zu16 *)image2.pixelAt(x+o, y))[0] = 0xFFFF;
            ((zu16 *)image2.pixelAt(x+o, y))[1] = 0xAAAA;
            ((zu16 *)image2.pixelAt(x+o, y))[2] = 0x0000;
        }
        ++o;
    }

    ZBinary out2;
    image2.setFormat(ZImage::PNG);
    image2.encodeFormat(out2);
    TASSERT(ZFile::writeBinary(IMAGE_TEST_16_PNG, out2));

    // Convert Transparent WebP to PNG

    ZBinary bin3i;
    ZFile::readBinary("dice.webp", bin3i);
    ZImage image3(bin3i);
    image3.setFormat(ZImage::PNG);
    ZBinary bin3o;
    image3.encodeFormat(bin3o);
    ZFile::writeBinary("dice.png", bin3o);

    return 0;
}
