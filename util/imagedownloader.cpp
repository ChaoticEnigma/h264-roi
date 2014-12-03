#include "zlog.h"
#include "zfile.h"
#include "zbinary.h"
using namespace LibChaos;

// Download multiple images from internet
// Args: <download prefix>

bool pixivDownload(ZString user_id, ZString session);
bool pixivBookmarksDownload(ZString user_id, ZString session);

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
            if(argc != 4){
                LOG("pixiv Usage: pixiv <user_id> <firefox_session>");
                return -1;
            }
            LOG("Downloading Works for pixiv User " << argv[2] << " with session " << argv[3]);
            // 27691
            pixivDownload(argv[2], argv[3]);
            LOG("Done Downloading");

        } else if(command == "pixivb"){ // Download all bookmarks from a pixiv user id
            if(argc != 4){
                LOG("pixiv Usage: pixiv <user_id> <firefox_session>");
                return -1;
            }
            LOG("Downloading Bookmarks for pixiv User " << argv[2] << " with session " << argv[3]);
            // 27691
            pixivBookmarksDownload(argv[2], argv[3]);
            LOG("Done Downloading");
        }
    }
    return 0;
}

ZString getPixivPage(ZString url, ZString session, ZString args = ""){
    ZString output = ZString(time(NULL)) + "-" + rand() + ".html";

    ZString agent = "--user-agent=\"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:33.0) Gecko/20100101 Firefox/33.0\"";
    ZString cookies = "--no-cookies --header=\"Cookie: PHPSESSID=" + session + "\"";
    ZString command = "wget --no-check-certificate " + /*agent + " " +*/ cookies + " " + args + " -O " + output + " \"" + url + "\"";

    if(system((command + "1> NUL 2> NUL").cc()) != 0){
        LOG("System command failed");
    } else {
//        LOG("Saved " << output);
    }

    ZString doc = ZFile::readString(output);
    ZFile::remove(output);
    return doc;
}

bool savePixivImage(ZString id, ZString user, ZString session, ZString &file){
    ZString mediumurl = "http://www.pixiv.net/member_illust.php?mode=medium&illust_id=" + id;
//    ZString bigurl = "http://www.pixiv.net/member_illust.php?mode=big&illust_id=" + id;

    ZString medpage = getPixivPage(mediumurl, session);
    ZString refbigurl = "http://www.pixiv.net/" + medpage.findFirstBetween("<div class=\"works_display\"><a href=\"", "\" target=\"_blank\" class=\"");
    refbigurl.replace("&amp;", "&");

    ZString pixivrefer = "--header=\"Referer: " + mediumurl + "\"";
    ZString imgpage = getPixivPage(refbigurl, session, pixivrefer);

    ZString imgurl;
    ZString type = refbigurl.findFirstBetween("mode=", "&");
    if(type == "big"){
        imgurl = imgpage.findFirstBetween("</head><body><img src=\"", "\" onclick=\"");
    } else if(type == "manga"){
        imgurl = imgpage.findFirstBetween("data-filter=\"manga-image\" data-src=\"", "\" data-index=\"");
    }

    ZString agent = "--user-agent=\"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:33.0) Gecko/20100101 Firefox/33.0\"";
    ZString cookies = "--no-cookies --header=\"Cookie: PHPSESSID=" + session + "\"";
//    ZString headers = "--header=\"Host: i4.pixiv.net\"";
    ZString headers = "--header=\"Host: " + imgurl.explode('/')[1] + "\"";
    ZString referer = "--header=\"Referer: " + refbigurl + "\"";
    ZString command = "wget --no-check-certificate " + /*agent + " " +*/ cookies + " " + headers + " " + referer + " -P " + user + " \"" + imgurl + "\"";

    if(system((command + "1> NUL 2> NUL").cc()) != 0){
        LOG("Failed: " << command);
        return false;
    } else {
        file = (ZPath(user) + ZPath(imgurl).last()).str();
        return true;
    }
}

bool pixivDownload(ZString user_id, ZString session){
    ArZ imglist;
    bool any = true;
    for(zu64 i = 1; any; ++i){
        any = false;
        ZString page = getPixivPage("http://www.pixiv.net/member_illust.php?id=" + user_id + "&type=all&p=" + i, session);
        page = page.findFirstBetween("<div class=\"\"><ul class=\"_image-items\">", "</ul></div><div class=\"clear\"></div><ul class=\"column-order-menu\"><div class=\"pager-container\">");
        ArZ list = page.strExplode("<li class=\"image-item\">");
        zu64 count = 0;
        for(zu64 j = 0; j < list.size(); ++j){
            ZString item = list[j];
            ZString test = "<a href=\"/member_illust.php?mode=medium&amp;illust_id=";
            if(item.startsWith(test)){
                item.substr(test.size());
                item = ZString::getUntil(item, "\"");
                if(item.isInteger()){
                    imglist.push(item);
                    ++count;
                    any = true;
                }
            }
        }
        LOG("Read page " << i << " - " << count);
    }
    LOG("Downloading " << imglist.size() << " images");

    for(zu64 i = 0; i < imglist.size(); ++i){
        ZString item = imglist[i];
        ZString file;

        int failcount = 0;
        while(!savePixivImage(item, user_id, session, file) && failcount < 5){
            ++failcount;
            LOG(i+1 << "/" << imglist.size() << " retrying " << item);
        }

        if(failcount == 0){
            LOG(i+1 << "/" << imglist.size() << " Saved " << item << " as " << file);
        } else {
            LOG(i+1 << "/" << imglist.size() << " Failed to download " << item);
        }
    }
    return true;
}

bool pixivBookmarksDownload(ZString user_id, ZString session){
    ArZ imglist;
    bool any = true;
    for(zu64 i = 1; any; ++i){
        any = false;
        ZString page = getPixivPage("http://www.pixiv.net/bookmark.php?id=" + user_id + "&rest=show&p=" + i, session);
        page = page.findFirstBetween("<div class=\"\"><ul class=\"_image-items\">", "</ul></div><div class=\"clear\"></div><ul class=\"column-order-menu\"><div class=\"pager-container\">");
        ArZ list = page.strExplode("<li class=\"image-item\" id=\"li_");
        zu64 count = 0;
        for(zu64 j = 0; j < list.size(); ++j){
            ZString item = list[j];
            item.substr(12);
            ZString test = "<a href=\"member_illust.php?mode=medium&amp;illust_id=";
            if(item.startsWith(test)){
                item.substr(test.size());
                item = ZString::getUntil(item, "\"");
                if(item.isInteger()){
                    imglist.push(item);
                    ++count;
                    any = true;
                }
            }
        }
        LOG("Read page " << i << " - " << count);
    }
    LOG("Downloading " << imglist.size() << " images");

    for(zu64 i = 0; i < imglist.size(); ++i){
        ZString item = imglist[i];
        ZString file;
        if(savePixivImage(item, user_id, session, file)){
            LOG(i+1 << "/" << imglist.size() << " Saved " << item << " as " << file);
        } else {
            LOG(i+1 << "/" << imglist.size() << " Failed to download " << item);
        }
    }
    return true;
}
