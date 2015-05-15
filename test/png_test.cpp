#include "test.h"

#include "zpng.h"
#include "zbmp.h"
#include "zfile.h"

int png_block(){

    LOG(ZPNG::libpngVersionInfo());

/*
    LOG("tmp4 bmp in");
    ZBMP bmp;
    LOG(bmp.read("tmp4-Z.bmp"));
    LOG(bmp.getImage().width() << " " << bmp.getImage().height() << " " << bmp.getImage().channels() << " " << bmp.getImage().depth());
    ZImage imgh(bmp.getImage());

    LOG("Equal: " << (imgh == bmp.getImage()));

    bmp.getImage().setHeight(bmp.getImage().height() / 2);
    //bmp.getImage().setHeight(bmp.getImage().height() * 2);
    bmp.getImage().setWidth(bmp.getImage().width() / 2);
    //bmp.getImage().setWidth(bmp.getImage().width() * 2);

    LOG("Equal: " << (imgh == bmp.getImage()));

    LOG(bmp.getImage().width() << " " << bmp.getImage().height() << " " << bmp.getImage().channels() << " " << bmp.getImage().depth());
    LOG("tmp4 bmp out");
    LOG(bmp.write("tmp4-out.bmp"));
    LOG(bmp.read("tmp4-out.bmp"));
    LOG(bmp.getImage().width() << " " << bmp.getImage().height() << " " << bmp.getImage().channels() << " " << bmp.getImage().depth());

    LOG("tmp4 png out 3");
    ZImage img;
    img.transferImage(imgh);
    ZPNG png(img);
    LOG(png.write("tmp4-out3.png"));
    LOG(png.read("tmp4-out3.png"));
    LOG(png.getImage().width() << " " << png.getImage().height() << " " << png.getImage().channels() << " " << png.getImage().depth());

    LOG("tmp4 png out 4");
    ZBinary zero;
    zero.fill(0, 4);
    zero.back() = 127;
    //img.setChannels(4);
    img.setChannels(4, zero.raw());
    ZPNG png1(img);
    LOG(png1.write("tmp4-out4.png"));
    LOG(png.read("tmp4-out4.png"));
    LOG(png.getImage().width() << " " << png.getImage().height() << " " << png.getImage().channels() << " " << png.getImage().depth());

    // Toucan
    LOG("toucan png in");
    ZPNG png2;
    LOG(png2.read("toucan.png"));
    LOG(png2.getImage().width() << " " << png2.getImage().height() << " " << png2.getImage().channels() << " " << png2.getImage().depth());
    ZImage imgh2(png2.getImage());

    png2.getImage().setHeight(png2.getImage().height() / 2);
    png2.getImage().setHeight(png2.getImage().height() * 2);
    png2.getImage().setWidth(png2.getImage().width() / 2);
    png2.getImage().setWidth(png2.getImage().width() * 2);

    LOG("toucan png out");
    LOG(png2.write("toucan-out.png"));

    LOG("toucan bmp out");
    imgh2.setChannels(3);
    ZBMP bmp2(imgh2);
    LOG(bmp2.write("toucan-out.bmp"));
*/

/*
    LOG("toucan bmp in");
    ZBMP bmp3;
    //bmp3.read("tmp4-Z.bmp");
    bmp3.read("toucan-out.bmp");
    LOG("toucan png out");
    ZPNG png3(bmp3.toBitmap().recast<PixelRGBA>(255));
    png3.pngText("Title") = "Test Title";
    png3.write("outp.png");
    //png3.read("outp.png");
    LOG("toucan resize");
    ZBitmapRGB bitmap1 = png3.toBitmap().recast<PixelRGB>();
    bitmap1.height(bitmap1.height() / 2);
    bitmap1.height(bitmap1.height() * 2);
    bitmap1.width(bitmap1.width() / 2);
    bitmap1.width(bitmap1.width() * 2);
    LOG("toucan bmp in");
    ZBMP bmp4(bitmap1);
    bmp4.write("toucan-reout.bmp");

    LOG("whois png in");
    ZPNG png4;
    png4.read("whois.png");
    LOG("whois bmp out");
    ZBMP bmp5(png4.toBitmap().recast<PixelRGB>());
    bmp5.write("whois-out.bmp");
    LOG("whois bmp in");
    ZBMP bmp6;
    bmp6.read("whois-out.bmp");
    LOG("whois png out");
    ZPNG png5(bmp6.toBitmap().recast<PixelRGBA>(255));
    png5.write("whois-reout.png");
*/

    ZArray<ZPath> files;
    //files = ZFile::listFiles("png");
    //files = { "png/f00n2c08.png" };
    files = { "png/PngSuite.png", "toucan.png" };
    //FOREACHIN(files, ZPath, file){
    for(zu64 i = 0; i < files.size(); ++i){
        ZPath file = files[i];
        LOG(file.last());

        // PNG parse
        ZBinary pngdata;
        ZFile::readBinary(file, pngdata);
        ZArray<ZPNG::PngChunk> chunks = ZPNG::parsePngAncillaryChunks(pngdata);
        for(zu64 j = 0; j < chunks.size(); ++j){
            //LOG("    " << chunks[j].size << " " << chunks[j].name);
            LOG("    " << chunks[j].size << " " << chunks[j].name << " " << chunks[j].data << " " << chunks[j].crc_ok);
        }

        //continue;

        // PNG read

        ZPNG pngin;
        try {
            pngin.read(file);
        } catch(ZException e){
            LOG("    Read Failed: " << e.what());
            continue;
        }

        ZImage imgin = pngin.getImage();
        ZFile::writeBinary("imgin-dump.bin", ZBinary(imgin.buffer(), imgin.realSize()));
        LOG("    " << imgin.width() << " " << imgin.height() << " " << imgin.channels() << " " << imgin.depth());

        // PNG write

        ZPath pngpath = ZPath("pngout/") + file.last();
        ZPNG pngout(imgin);
        try {
            pngout.write(pngpath);
        } catch(ZException e){
            LOG("    PNG Write Failed: " << e.what());
            continue;
        }

        ZPNG tmppngin;
        tmppngin.read(pngpath);
        LOG("    " << tmppngin.getImage().width() << " " << tmppngin.getImage().height() << " " << tmppngin.getImage().channels() << " " << tmppngin.getImage().depth());
        LOG("    " << ((tmppngin.getImage() == imgin) ? "++++++++" : "--------"));

        // BMP write

        if(!imgin.isRGB24() && !imgin.isRGBA32()){
            //if(imgin.isRGB48() || imgin.isRGBA64()){
            //    LOG("    Strip 16 to 8");
            //    imgin.strip16to8bit();
            //} else {
                LOG("    Cannot Write to BMP: " << imgin.channels() << "," << imgin.depth());
                continue;
            //}
        }

        if(imgin.isRGBA32()){
            LOG("    Drop alpha channel");
            imgin.setChannels(3);
        }

        //LOG(files[i].last() << " " << imgin.width() << " " << imgin.height() << " " << imgin.channels() << " " << imgin.depth());
        //imgin.setDepth(8);
        //LOG(files[i].last() << " " << imgin.width() << " " << imgin.height() << " " << imgin.channels() << " " << imgin.depth());

        ZPath bmppath = ZPath("bmpout") + file.last().replace(".png", ".bmp");
        ZBMP tmp(imgin);
        if(!tmp.write(bmppath)){
            LOG("    BMP Write Failed: " << tmp.getError().what());
            continue;
        }

        ZBMP tmpin;
        if(!tmpin.read(bmppath)){
            LOG("    BMP Read Failed: " << tmpin.getError().what());
        }
        LOG("    " << tmpin.getImage().width() << " " << tmpin.getImage().height() << " " << tmpin.getImage().channels() << " " << tmpin.getImage().depth());
        LOG("    " << ((tmpin.getImage() == imgin) ? "++++++++" : "--------"));

    }

    return 0;
}
