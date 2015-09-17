#include "test.h"
#include "zimage.h"
#include "zbmp.h"
#include "zppm.h"
#include "zpng.h"

int image_test(){

    // 8-bit

    zu64 width = 1920;
    zu64 height = 1080;
    LOG("Creating " << width << "x" << height << " image");
    ZImage image1(width, height, ZImage::rgb24);
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

    ZPNG outpng1(image1);
    LOG("PNG: " << outpng1.write("image_test.png"));

    ZBMP outbmp1(image1);
    LOG("BMP: " << outbmp1.write("image_test.bmp"));

    ZPPM outppm1(image1);
    LOG("PPM: " << outppm1.write("image_test.ppm"));

    // 16-bit

    LOG("Creating " << width << "x" << height << " image");
    ZImage image2(width, height, ZImage::rgb48);
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

    ZPNG outpng2(image2);
    LOG("PNG: " << outpng2.write("image_test16.png"));

    return 0;
}
