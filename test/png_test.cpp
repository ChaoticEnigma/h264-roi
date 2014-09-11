#include "test.h"

#include "zpng.h"
#include "zbmp.h"
#include "zfile.h"

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

    ZBMP bmp3;
    //bmp3.read("tmp4-Z.bmp");
    bmp3.read("toucan-out.bmp");

    ZPNG png3(bmp3.toBitmap().recast<PixelRGBA>(255));
    png3.pngText("Title") = "Test Title";
    png3.write("outp.png");

    //png3.read("outp.png");

    ZBitmapRGB bitmap1 = png3.toBitmap().recast<PixelRGB>();
    bitmap1.height(bitmap1.height() / 2);
    bitmap1.height(bitmap1.height() * 2);
    bitmap1.width(bitmap1.width() / 2);
    bitmap1.width(bitmap1.width() * 2);

    ZBMP bmp4(bitmap1);
    bmp4.write("toucan-reout.bmp");

    // whois
    ZPNG png4;
    png4.read("whois.png");

    ZBMP bmp5(png4.toBitmap().recast<PixelRGB>());
    bmp5.write("whois-out.bmp");

    ZBMP bmp6;
    bmp6.read("whois-out.bmp");

    ZPNG png5(bmp6.toBitmap().recast<PixelRGBA>(255));
    png5.write("whois-reout.png");

    return 0;

//    ZPNG png4;
//    ZArray<ZPath> files = ZFile::listFiles("png");
//    for(zu64 i = 0; i < files.size(); ++i){
//        if(png4.read(files[i])){
//            ZBMP tmp(png4.toBitmap().recast<PixelRGB>());
//            tmp.write(ZPath("bmp/") + files[i].last().replace(".png", ".bmp"));
//        } else {
//            LOG("Failed: " << files[i]);
//        }
//    }
    //png4.read("png/basi0g01.png");
    //png4.read("png/basi4a08.png");

    return 0;
}
