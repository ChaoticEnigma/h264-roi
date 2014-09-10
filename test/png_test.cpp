#include "test.h"

#include "zpng.h"
#include "zbmp.h"

int png_block(){

    LOG(ZPNG::libpngVersionInfo());

    ZBMP bmp;
    bmp.read("tmp4-Z.bmp");
    bmp.write("tmp-out.bmp");

    ZPNG png2;
    png2.read("toucan.png");
    //png2.read("whois.png");
    ZBMP bmp2(png2.toBitmap().recast<PixelRGB>());
    bmp2.write("toucan-out.bmp");

//    ZBMP bmp3;
//    bmp3.read("tmp4-Z.bmp");

//    ZPNG png3(bmp3.toBitmap());
//    png3.write("outp.png");

    return 0;
}
