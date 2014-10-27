#include "zlogworker.h"
#include "zfile.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstdlib>

namespace LibChaos {

ZMutex ZLogWorker::mtx;
ZMutex ZLogWorker::formatMtx;
ZCondition ZLogWorker::cond;
std::queue<ZLogWorker::LogJob> ZLogWorker::jobs;

ZMap<ZLogWorker::zlog_source, ZString> ZLogWorker::stdoutlog;
ZMap<ZLogWorker::zlog_source, ZString> ZLogWorker::stderrlog;
ZAssoc< ZPath, ZMap<ZLogWorker::zlog_source, ZString> > ZLogWorker::logfiles;
bool ZLogWorker::lastcomp;

ZLogWorker::ZLogWorker(){
    //work = work(zlogWorker);

    // Output buffering can be default, since it is flushed each line anyway
    //setbuf(stdout, NULL);

    //    formatStdout(ZLogSource::normal, TIMETHREAD); // These cause a memory leak...?
    //    formatStderr(ZLogSource::error, DETAILLOG);
}

ZLogWorker::~ZLogWorker(){
    if(work.tid())
        waitEnd();
}

void ZLogWorker::run(){
    work.run(zlogWorker);
}

void ZLogWorker::waitEnd(){ // Must NEVER be called by log worker thread
    work.stop();
    cond.signal();
    work.join();
}

void ZLogWorker::queue(LogJob lj){
    mtx.lock();
    jobs.push(lj);
    mtx.unlock();
    cond.signal();
}

void *ZLogWorker::zlogWorker(void *arg){
    ZThreadArg *zarg = (ZThreadArg*)arg;
    std::queue<LogJob> tmp;
    while(true){
        mtx.lock(); // Lock mutex
        if(jobs.empty()){ // If no jobs, wait for jobs
            if(zarg->stop()){ // If stopped, just break
                mtx.unlock();
                break;
            }

            // Wait to be woken up with work to do
            cond.waitLock();
            while(jobs.empty()){
                mtx.unlock(); // Unlock while waiting so work can be queued
                cond.wait();
                mtx.lock(); // Lock again before while(jobs.empty()) condition
            }
            cond.waitUnlock();
        }
        jobs.swap(tmp); // Swap queues
        mtx.unlock(); // Done with jobs

        // Work through temporary queue
        while(!tmp.empty()){
            doLog(tmp.front());
            tmp.pop();
        }

        ZThread::yield(); // Yield to other threads
    }
    return NULL;
}

ZString ZLogWorker::makeLog(LogJob &job, ZString f){
    ZString fmt = f;
    if(!job.raw){
        fmt.replace("%log%", job.log);
        fmt.replace("%clock%", job.pinfo[clock]);
        fmt.replace("%date%", job.pinfo[date]);
        fmt.replace("%time%", job.pinfo[time]);
        fmt.replace("%datetime%", job.pinfo[time] + " " + job.pinfo[date]);
        fmt.replace("%thread%", job.pinfo[thread]);
        fmt.replace("%line%", job.pinfo[line]);
        fmt.replace("%file%", job.pinfo[file]);
        fmt.replace("%function%", job.pinfo[function]);
        if(job.newln && fmt.last() != '\n')
            fmt << '\n';
        return fmt;
    } else {
        return job.log;
    }
}

void ZLogWorker::doLog(LogJob &jb){
    formatMtx.lock();
    if(!stdoutlog[jb.source].isEmpty()){
        fprintf(stdout, "%s", makeLog(jb, stdoutlog[jb.source]).cc());
        fflush(stdout);
    } else if(!stdoutlog[ZLogSource::all].isEmpty()){
        fprintf(stdout, "%s", makeLog(jb, stdoutlog[ZLogSource::all]).cc());
        fflush(stdout);
    }

    if(!stderrlog[jb.source].isEmpty()){
        fprintf(stderr, "%s", makeLog(jb, stderrlog[jb.source]).cc());
        fflush(stderr);
    } else if(!stderrlog[ZLogSource::all].isEmpty()){
        fprintf(stderr, "%s", makeLog(jb, stderrlog[ZLogSource::all]).cc());
        fflush(stderr);
    }

    if(!jb.stdio){
        for(zu64 i = 0; i < logfiles.size(); ++i){
            if(!logfiles[i][jb.source].isEmpty()){
                ZFile::createDirsTo(logfiles.key(i));

                std::ofstream lgfl(logfiles.key(i).str().cc(), std::ios::app);
                lgfl << makeLog(jb, logfiles[i][jb.source]);
                lgfl.flush();
                lgfl.close();
            } else if(!logfiles[i][ZLogSource::all].isEmpty()){
                ZFile::createDirsTo(logfiles.key(i));

                std::ofstream lgfl(logfiles.key(i).str().cc(), std::ios::app);
                lgfl << makeLog(jb, logfiles[i][ZLogSource::all]);
                lgfl.flush();
                lgfl.close();
            }
        }
    }
    formatMtx.unlock();
}

void ZLogWorker::formatStdout(zlog_source type, ZString fmt){
    formatMtx.lock();
    stdoutlog[type] = fmt;
    formatMtx.unlock();
}
void ZLogWorker::formatStderr(zlog_source type, ZString fmt){
    formatMtx.lock();
    stderrlog[type] = fmt;
    formatMtx.unlock();
}
void ZLogWorker::addLogFile(ZPath pth, zlog_source type, ZString fmt){
    formatMtx.lock();
    logfiles[pth][type] = fmt;
    formatMtx.unlock();
}

}
