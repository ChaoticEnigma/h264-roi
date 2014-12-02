#include "zlog.h"
#include "zfile.h"
#include "zbinary.h"
using namespace LibChaos;

// Download multiple images from internet
// Args: <download prefix>

bool pixivDownload(ZString user_id);

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::all, "%log%");

    if(argc > 1){
        ZString command = argv[1];
        if(command == "numloop"){  // Download images from a url with an incrementable number
            ArZ param_args;
            AsArZ param_flags;
            for(int i = 0; i < argc; ++i){
                if(argv[i][0] == '-'){
                    ZString tmp = argv[i];
                    tmp.substr(1);
                    ArZ fl = tmp.explode('=');
                    ZString flg = fl[0];
                    fl.popFront();
                    ZString str = ZString::compound(fl, ZString());
                    param_flags[flg] = str;
                } else {
                    param_args.push(argv[i]);
                }
            }
            param_args.popFront();

            LOG("Downloading Images...");
            ZString url;
            url = "http://www.ietf.org/rfc/rfc4122.txt";
            ZString cmd = "wget --no-check-certificate -P ";
            cmd += param_args[0];
            cmd += " ";

            ZFile::makeDir(param_args[0]);

            ZString cd = "cd ";
            cd += param_args[0];
            LOG(cd);
            system(cd.cc());

            ZString cmdstr;
            zu64 n = 1;
            do {
                cmdstr = cmd + url + ZString::ItoS(n, 10, 3) + ".jpg";
                LOG(cmdstr);
                ++n;
            } while(system(cmdstr.cc()) == 0);

            LOG("Finished");

        } else if(command == "pixiv"){ // Download all works from a pixiv user id
            if(argc != 3){
                LOG("Incorrent arguments for pixiv");
                return -1;
            }
            LOG("Downloading Works for pixiv User " << argv[2]);
            pixivDownload(argv[2]);
            LOG("Done Downloading");
        }
    }
    return 0;
}

ZString getPixivPage(ZString url, ZString refer = ""){
    ZString output = ZString(time(NULL)) + "-" + rand();
//    ZString cookiestr = "p_ab_id=4;login_ever=yes;lang=en;pixiv_embed=pix;bookmark_tag_type=count;bookmark_tag_order=desc;staccMoreAutoViewIsEnabled=1;staccLatestAutoViewIsEnabled=0;stacc_mode=unify;module_orders_mypage=%5B%7B%22name%22%3A%22everyone_new_illusts%22%2C%22visible%22%3Atrue%7D%2C%7B%22name%22%3A%22spotlight%22%2C%22visible%22%3Atrue%7D%2C%7B%22name%22%3A%22featured_tags%22%2C%22visible%22%3Atrue%7D%2C%7B%22name%22%3A%22contests%22%2C%22visible%22%3Atrue%7D%2C%7B%22name%22%3A%22following_new_illusts%22%2C%22visible%22%3Atrue%7D%2C%7B%22name%22%3A%22mypixiv_new_illusts%22%2C%22visible%22%3Atrue%7D%2C%7B%22name%22%3A%22booth_follow_items%22%2C%22visible%22%3Atrue%7D%5D;GCSCU_89993436389_H3=C=89993436389.apps.googleusercontent.com:S=2e1a265dfd2053a9fcd2cb0a6ee99617b51a64d3.FwKYIqjgxQWk3Va7.6103:I=1417517136:X=1417603536;PHPSESSID=12718621_3bad59190ca3e6ce47e0d60b06c9dd2b";
    ZString cookies = "PHPSESSID=12718621_3bad59190ca3e6ce47e0d60b06c9dd2b";
    ZString command = "wget --no-check-certificate --no-cookies --header=\"Cookie: " + cookies + "\" " + (refer.isEmpty() ? "" : "--header=\"Referer: " + refer + "\" ") + "-O " + output + " \"" + url + "\"";

//    LOG(command);
    if(system((command + "1> NUL 2> NUL").cc()) != 0){
        LOG("System command failed");
    } else {
//        LOG("Saved " << output);
    }

    ZString doc = ZFile::readString(output);
    return doc;
}

bool savePixivImage(ZString id){
    //LOG("Saving " << id);
    ZString imgpage = getPixivPage("http://www.pixiv.net/member_illust.php?mode=big&illust_id=" + id, "http://www.pixiv.net/member_illust.php?mode=medium&illust_id=" + id);
    imgpage = imgpage.findFirstBetween("</head><body><img src=\"", "\" onclick=\"");
    //LOG("URL " << imgpage);

    ZString cookies = "PHPSESSID=12718621_3bad59190ca3e6ce47e0d60b06c9dd2b";
    ZString command = "wget --no-check-certificate --user-agent=\"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:33.0) Gecko/20100101 Firefox/33.0\" --no-cookies --header=\"Cookie: " + cookies + "\" --header=\"Host: i4.pixiv.net\" --header=\"Referer: http://www.pixiv.net/member_illust.php?mode=big&illust_id=" + id + "\" \"" + imgpage + "\"";

//    LOG(command);
    if(system((command + "1> NUL 2> NUL").cc()) != 0){
        LOG("System command failed");
        return false;
    } else {
        ZPath hack = imgpage;
        LOG("Saved Image: " << hack.last());
        return true;
    }
}


bool pixivDownload(ZString user_id){
    // 27691
    int count = 1;
    for(zu64 ip = 1; count > 0; ++ip){
        count = 0;
//        ZString one = getPixivPage("http://www.pixiv.net/member_illust.php?id=" + user_id + "&type=all&p=" + ip);
        ZString one = getPixivPage("http://www.pixiv.net/bookmark.php?id=" + user_id + "&rest=show&p=" + ip);

        one = one.findFirstBetween("<div class=\"\"><ul class=\"_image-items\">", "</ul></div><div class=\"clear\"></div><ul class=\"column-order-menu\"><div class=\"pager-container\">");
//        ArZ list = one.strExplode("<li class=\"image-item\">");
        ArZ list = one.strExplode("<li class=\"image-item\" id=\"li_");

        for(zu64 i = 0; i < list.size(); ++i){
            ZString item = list[i];
            item.substr(12);
//            ZString test = "<a href=\"/member_illust.php?mode=medium&amp;illust_id=";
            ZString test = "<a href=\"member_illust.php?mode=medium&amp;illust_id=";
            if(item.startsWith(test)){
                item.substr(test.size());
                item = ZString::getUntil(item, "\"");
                LOG("Image: " << item);
                savePixivImage(item);
                ++count;
            }
        }
    }

    return true;
}
