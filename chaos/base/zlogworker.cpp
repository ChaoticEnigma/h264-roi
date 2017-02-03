/*******************************************************************************
**                                  LibChaos                                  **
**                               zlogworker.cpp                               **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#include "zlogworker.h"
#include "zlog.h"
#include "zfile.h"
#include "zqueue.h"
#include "zmap.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstdlib>

namespace LibChaos {

ZMutex jobmutex;
ZCondition jobcondition;
ZQueue<ZLogWorker::LogJob*> jobs;

//ZMutex writemutex;

struct LogHandler {
    enum lhtype {
       STDOUT, STDERR, FILE
    };
    LogHandler(lhtype t, ZString f) : type(t), fmt(f){}
    lhtype type;
    ZString fmt;
    ZPath file;
};

ZMutex formatmutex;
ZMap<int, ZList<LogHandler>> logformats;

ZMutex threadidmutex;
ZMap<ztid, zu32> threadids;

bool ZLogWorker::lastcomp;

bool enablestdout = true;
bool enablestderr = true;

ZLogWorker::ZLogWorker(){
//    work = work(zlogWorker);

    // Output buffering can be default, since it is flushed each line anyway
//    setbuf(stdout, NULL);

//    formatStdout(ZLogSource::normal, TIMETHREAD); // These cause a memory leak...?
//    formatStderr(ZLogSource::error, DETAILLOG);
}

ZLogWorker::~ZLogWorker(){
    if(work.tid()){
        stop();
    }
}

void ZLogWorker::run(){
    work.run(zlogWorker);
}

void ZLogWorker::stop(){ // Must NEVER be called by log worker thread
    // BUG: Sometimes hangs?
    work.stop();
    jobcondition.signal(); // Wake up thread
    work.join();
}

void ZLogWorker::queue(LogJob *job){
    jobmutex.lock();
    jobs.push(job);
    jobmutex.unlock();
    jobcondition.signal();
}

void *ZLogWorker::zlogWorker(ZThreadArg *zarg){
    ZQueue<LogJob*> tmp;
    while(true){
        jobmutex.lock(); // Lock mutex
        if(jobs.isEmpty()){ // If no jobs, wait for jobs
            if(zarg->stop()){ // If stopped, just break
                jobmutex.unlock();
                break;
            }

            // Wait to be woken up with work to do
            jobcondition.waitLock();
            while(jobs.isEmpty()){
                jobmutex.unlock(); // Unlock while waiting so work can be queued
                jobcondition.wait();
                jobmutex.lock(); // Lock again before jobs.empty() check
            }
            jobcondition.waitUnlock();
        }
        jobs.swap(tmp); // Swap queues
        jobmutex.unlock(); // Done with jobs

        // Work through temporary queue
        while(!tmp.isEmpty()){
            doLog(tmp.peek());
            tmp.pop();
        }

        ZThread::yield(); // Yield to other threads
    }
    return NULL;
}

ZString ZLogWorker::getThread(ztid thread){
    zu16 id;
    threadidmutex.lock();
    if(threadids.contains(thread)){
        id = threadids[thread];
    } else {
        id = threadids.size();
        threadids.push(thread, id);
    }
    threadidmutex.unlock();
    return ZString(id);
}

ZString ZLogWorker::makeLog(const LogJob *job, ZString fmt){
    if(!job->raw){
        fmt.replace("%log%", job->log);

        fmt.replace("%clock%", job->clock.str());
        fmt.replace("%date%", job->time.dateStr());
        fmt.replace("%time%", job->time.timeStr());
        fmt.replace("%datetime%", job->time.dateStr() + " " + job->time.timeStr());
        fmt.replace("%thread%", getThread(job->thread));

        fmt.replace("%file%", job->file);
        fmt.replace("%line%", job->line);
        fmt.replace("%function%", job->func);

        if(job->newln && fmt.last() != '\n')
            fmt += '\n';
        return fmt;
    } else {
        return job->log;
    }
}

void ZLogWorker::doLog(LogJob *job){
    formatmutex.lock();
    ZList<LogHandler> formats = logformats[job->level];
    formatmutex.unlock();

    for(zu64 i = 0; i < formats.size(); ++i){
        switch(formats.front().type){
            case LogHandler::STDOUT:
                if(enablestdout){
                    fprintf(stdout, "%s", makeLog(job, formats.front().fmt).cc());
                    fflush(stdout);
                }
                break;
            case LogHandler::STDERR:
                if(enablestderr){
                    fprintf(stderr, "%s", makeLog(job, formats.front().fmt).cc());
                    fflush(stderr);
                }
                break;
            case LogHandler::FILE: {
                ZFile::createDirsTo(formats.front().file);
                std::ofstream lgfl(formats.front().file.str().cc(), std::ios::app);
                lgfl << makeLog(job, formats.front().fmt);
                lgfl.flush();
                lgfl.close();
                break;
            }
            default:
                break;
        }

        // Rotate list
        formats.rotate();
    }

    // Destroy the job
    delete job;
}

void ZLogWorker::logLevelStdOut(int level, ZString fmt){
    formatmutex.lock();
    LogHandler lh(LogHandler::STDOUT, fmt);
    logformats[level].push(lh);
    formatmutex.unlock();
}
void ZLogWorker::logLevelStdErr(int level, ZString fmt){
    formatmutex.lock();
    LogHandler lh(LogHandler::STDERR, fmt);
    logformats[level].push(lh);
    formatmutex.unlock();
}
void ZLogWorker::logLevelFile(int level, ZPath file, ZString fmt){
    formatmutex.lock();
    LogHandler lh(LogHandler::FILE, fmt);
    lh.file = file;
    logformats[level].push(lh);
    formatmutex.unlock();
}

void ZLogWorker::setStdOutEnable(bool set){
    enablestdout = set;
}

void ZLogWorker::setStdErrEnable(bool set){
    enablestderr = set;
}

}
