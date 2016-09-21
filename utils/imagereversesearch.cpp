#include "zlog.h"
#include "zfile.h"
#include "zbinary.h"
#include "zmap.h"
using namespace LibChaos;

#include <openssl/md5.h>
#include <curl/curl.h>

// Find image sources

size_t write_callback(void *buffer, size_t size, size_t nmemb, void *userp){
   return size * nmemb;
}

size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata){
    ZBinary bin(buffer, size * nitems);
    bin.nullTerm();
    bin.printable();
    LOG(size << " " << nitems << " " << bin.asChar());
    return size * nitems;
}

int main(int argc, char **argv){
    try {
    ZLog::logLevelStdOut(ZLog::INFO, "%log%");

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(!curl){
        curl_global_cleanup();
        return EXIT_FAILURE;
    }

    zu64 count = 0;
    if(argc < 2){
        LOG("Usage: imagereversesearch <dir>");
        return EXIT_FAILURE;
    }
    ZPath search = argv[1];
    LOG("Searching for images in " << search);
    ZArray<ZPath> files = ZFile::listFiles(search, true);
    LOG("Testing " << files.size() << " files");

    MD5_CTX mdcontext;
    zbyte mdc[MD5_DIGEST_LENGTH];

    ZMap<ZString, ZString> hashtable;
    for(zu64 i = 0; i < files.size(); ++i){
        ZPath path = files[i];
        ZBinary bin;
        ZFile::readBinary(path, bin);
        MD5_Init(&mdcontext);
        MD5_Update(&mdcontext, bin.raw(), bin.size());
        MD5_Final(mdc, &mdcontext);
        ZString mdhash;
        for(zu64 i = 0; i < MD5_DIGEST_LENGTH; ++i)
            mdhash += ZString::ItoS(mdc[i], 16, 2);
        hashtable.add(files[i].str(), mdhash);
        //LOG(files[i] << " " << mdhash);

        long http_code = 0;

        ZString url = "http://e621.net/post/show.json?md5=" + mdhash;
        LOG(files[i] << " " << url << " " << ZLog::NOLN);

        curl_easy_setopt(curl, CURLOPT_URL, url.cc());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        //curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
        //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        if(res == CURLE_OK){
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if(http_code == 200){
                LOG("HIT e621 " << http_code);
                ++count;
            } else {
                LOG("miss e621 " << http_code);
            }
        } else {
            LOG("failed e621: " << curl_easy_strerror(res));
        }

        //curl_easy_setopt(curl, CURLOPT_URL, ZString("http://rule34.paheal.net/post/list/md5:" + mdhash + "/1").cc());
        //res = curl_easy_perform(curl);
        //if(res == CURLE_OK){
        //    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        //    if(http_code == 302){
        //        LOG("HIT rule34 " << http_code);
        //        ++count;
        //    } else {
        //        LOG("miss rule34");
        //    }
        //} else {
        //    LOG("failed rule34");
        //}

    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    LOG("Finished: " << count << " search hits");
    } catch(ZException e){
        LOG(e.what());
    }
    return 0;
}

