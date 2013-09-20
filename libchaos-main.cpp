#include <iostream>
#include "zstring.h"
#include "zfile.h"
#include "zlog.h"
#include "zpath.h"
#include "zthread.h"
#include "zarray.h"
#include "zassoc.h"

//using namespace std;
using namespace LibChaos;

void string_block(){
    /*ZString t3 = "hello, this is a string";
    cout << t3.size() << " " << t3 << endl;
    t3.append(", and this is appended");
    cout << t3.size() << " " << t3 << endl;
    t3.replace(" is ", " \"is still\" ");
    cout << t3.size() << " " << t3 << endl;
    ZArray<ZString> words = t3.explode(' ');
    for(unsigned long i = 0; i < words.size(); ++i)
        cout << '-' << words[i] << "- ";
    cout << endl;
    words.concat(t3.smart_explode(' '));
    for(unsigned long i = 0; i < words.size(); ++i)
        cout << '-' << words[i] << "- ";
    cout << endl;*/
    LOG("this text here");
    ZString tst;
    tst << 'a';
    tst << '5';
    tst << ' ';
    tst << 'g';
    tst << "7";
    LOG(tst);
    LOG("more text here");
}

void path_block(){
    // /a/path/here/test.txt
    // /this/path/to/another/place
    // ../../../../../a/path/here/test.txt

    // /some/short/thing.obj
    // /some/short/other/path
    // ../../thing.obj

    ZPath p1 = ZString("/some/odd/../complic/ated/../path");
    ZPath p2 = ZString("/"); // ../../thing.obj
    p1.relTo(p2);
    LOG(p1);
    LOG(p2);

    ZPath pc = ZString("/some/odd/../complic/ated/../path");
    pc.sanitize();
    LOG(pc);
    pc = "../../another/path";
    pc.sanitize();
    LOG(pc);
    pc = "/some/../../../path";
    pc.sanitize();
    LOG(pc);
}

void *thread_func(void *zarg){
    ZThreadArg *arg = (ZThreadArg*)zarg;
    LOG("running " << ZThread::thisTid());
    sleep(2);
    LOG("waited 2 " << ZThread::thisTid());
}
void *thread_func2(void *zarg){
    ZThreadArg *arg = (ZThreadArg*)zarg;
    LOG("running " << ZThread::thisTid());
    LOG((const char *)arg->arg << ", " << arg->stop());
    int i = 0;
    while(!arg->stop()){
        LOG("loop" << ++i << " in " << ZThread::thisTid());
        sleep(1);
    }
    LOG("broke loop " << ZThread::thisTid());
}
void thread_block(){
    /*
    LOG("this text " << ZThread::thisTid());
    ZThread thr(thread_func);
    LOG("thread " << thr.tid() << " created");
    sleep(1);
    LOG("waited 1 " << ZThread::thisTid());
    thr.kill();
    LOG("killed " << thr.tid());
    */

    const char *txt = "hello there from here";
    ZThread thr2(thread_func2, (void *)txt);
    LOG("thread " << thr2.tid() << " created");
    sleep(5);
    LOG("waited 5 " << ZThread::thisTid());
    thr2.stop();
    LOG("stopped " << thr2.tid());
    thr2.join();
    LOG("joined " << thr2.tid());

}

void file_block(){
    ZPath near = ZString("here");
    LOG(ZPath::makeDir(near));

    ZPath far = ZString("a/b/c/d/e/f.dat");
    LOG(far.createDirsTo());
}

void array_block(){
    ZArray<int> tst;
    tst.push(10);
    tst.pushFront(45);
    tst.push(567);
    for(unsigned i = 0; i < tst.size(); ++i){
        LOG(tst[i]);
    }

    ZAssoc<ZString, ZString> zarr;
    zarr["here"] = "there";
    zarr.pushFront("this", "that");
    zarr.push("where?");
    for(unsigned i = 0; i < zarr.size(); ++i){
        LOG(zarr.key(i) << " " << zarr[i]);
    }
}

int main(){
    ZLog::formatStdout(ZlogFormat(true, true, 1, true), ZlogFormat(false, true, 0, true), ZlogFormat(true, true, 0, true));
    ZLog::addLogFile(ZString("logs") + ZLog::genLogFileName("libchaos_"), ZlogFormat(true, true, 1, true), ZlogFormat(true, true, 1, true), ZlogFormat(true, true, 0, true));
    LOG("Starting libchaos Test");

    //string_block();
    //path_block();
    //thread_block();
    //file_block();
    array_block();

    return 0;
}
