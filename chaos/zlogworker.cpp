#include "zlogworker.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstdlib>

namespace LibChaos {

ZMutex<char> ZLogWorker::mtx;
ZMutex<char> ZLogWorker::formatMtx;
std::queue<ZLogWorker::LogJob> ZLogWorker::jobs;

std::atomic<bool> ZLogWorker::pending(false);
ZMap<zu64, ZString> ZLogWorker::stdoutlog;
ZMap<zu64, ZString> ZLogWorker::stderrlog;
ZAssoc< ZPath, ZMap<zu64, ZString> > ZLogWorker::logfiles;
bool ZLogWorker::lastcomp;

ZLogWorker::ZLogWorker(){
    //work = work(zlogWorker);
    setbuf(stdout, NULL);
    formatStdout(ZLogSource::normal, TIMETHREAD);
    formatStderr(ZLogSource::error, DETAILLOG);
}

ZLogWorker::~ZLogWorker(){
    //std::cout << "zlogworker" << std::endl;
    work.stop();
    work.join();
    //work.interrupt();
    //pthread_cancel(work);
    //wait();
}

int ZLogWorker::run(){
    work.run(zlogWorker);
    return 0;
}

void ZLogWorker::wait(){ // Must NEVER be called by log worker thread
    while(true){
        if(!pending && mtx.trylock()){
            mtx.unlock();
            if(jobs.empty()){
                break;
            }
        }
    }
    //pthread_join(work, NULL);
    //work.join();
}

void ZLogWorker::queue(LogJob lj){
    pending = true;
    mtx.lock();
    jobs.push(lj);
    mtx.unlock();
}

void *ZLogWorker::zlogWorker(void *zarg){
    ZThreadArg *arg = (ZThreadArg*)zarg;
    //std::cout << "worker work is working..." << std::endl;
    while(!arg->stop()){
        if(mtx.trylock()){
            if(!jobs.empty()){
                pending = true;
                std::queue<LogJob> tmp;
                jobs.swap(tmp);
                mtx.unlock();

                while(!tmp.empty()){
                    doLog(tmp.front());
                    tmp.pop();
                }
                pending = false;
            } else {
                mtx.unlock();
            }
        }
        //std::this_thread::yield();
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return NULL;
}

ZString ZLogWorker::makeLog(LogJob &job, ZString fmt){
    if(!job.raw){
        fmt.replace("%log%", job.log);
        fmt.replace("%clock%", job.pinfo[clock]);
        fmt.replace("%time%", job.pinfo[time]);
        fmt.replace("%thread%", job.pinfo[thread]);
        fmt.replace("%line%", job.pinfo[line]);
        fmt.replace("%file%", job.pinfo[file]);
        fmt.replace("%function%", job.pinfo[function]);
        if(job.newln)
            fmt << '\n';
        return fmt;
    } else {
        return job.log;
    }
}

void ZLogWorker::doLog(LogJob jb){
    formatMtx.lock();
    if(!stdoutlog[jb.source].isEmpty())
        printf("%s", makeLog(jb, stdoutlog[jb.source]).cc());

    if(!stderrlog[jb.source].isEmpty())
        fprintf(stderr, "%s", makeLog(jb, stderrlog[jb.source]).cc());

    if(!jb.stdio){
        for(zu64 i = 0; i < logfiles.size(); ++i){
            if(!logfiles[i][jb.source].isEmpty()){
                logfiles.key(i).createDirsTo();

                std::ofstream lgfl(logfiles.key(i).str().cc(), std::ios::app);
                lgfl << makeLog(jb, logfiles[i][jb.source]);
                lgfl.flush();
                lgfl.close();
            }
        }
    }
    formatMtx.unlock();
}

void ZLogWorker::formatStdout(zu64 type, ZString fmt){
    formatMtx.lock();
    stdoutlog[type] = fmt;
    formatMtx.unlock();
}
void ZLogWorker::formatStderr(zu64 type, ZString fmt){
    formatMtx.lock();
    stderrlog[type] = fmt;
    formatMtx.unlock();
}
void ZLogWorker::addLogFile(ZPath pth, zu64 type, ZString fmt){
    formatMtx.lock();
    logfiles[pth][type] = fmt;
    formatMtx.unlock();
}

}
