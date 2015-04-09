/*******************************************************************************
**                                  LibChaos                                  **
**                               zlogworker.cpp                               **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#include "zlogworker.h"
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

ZMutex formatmutex;
ZMap<ZLogWorker::zlog_source, ZString> stdoutlog;
ZMap<ZLogWorker::zlog_source, ZString> stderrlog;
ZMap<ZPath, ZMap<ZLogWorker::zlog_source, ZString> > logfiles;

ZMutex threadidmutex;
ZMap<ztid, zu32> threadids;

bool ZLogWorker::lastcomp;

ZLogWorker::ZLogWorker(){
//    work = work(zlogWorker);

    // Output buffering can be default, since it is flushed each line anyway
//    setbuf(stdout, NULL);

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
    // BUG: Sometimes hangs?
    work.stop();
    jobcondition.signal();
    work.join();
}

void ZLogWorker::queue(LogJob *job){
    jobmutex.lock();
    jobs.push(job);
    jobmutex.unlock();
    jobcondition.signal();
}

void *ZLogWorker::zlogWorker(void *arg){
    ZThreadArg *zarg = (ZThreadArg*)arg;
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
    if(threadids.exists(thread)){
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

        fmt.replace("%clock%", job->clock.getClock());
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
    ZString stdoutfmt = stdoutlog[job->source];
    ZString stdoutfmtall = stdoutlog[ZLogSource::all];
    ZString stderrfmt = stderrlog[job->source];
    ZString stderrfmtall = stderrlog[ZLogSource::all];
    formatmutex.unlock();

    // Do any stdout logging
    if(!stdoutfmt.isEmpty()){
        fprintf(stdout, "%s", makeLog(job, stdoutfmt).cc());
        fflush(stdout);
    } else if(!stdoutfmtall.isEmpty()){
        fprintf(stdout, "%s", makeLog(job, stdoutfmtall).cc());
        fflush(stdout);
    }

    // Do any stderr logging
    if(!stderrfmt.isEmpty()){
        fprintf(stderr, "%s", makeLog(job, stderrfmt).cc());
        fflush(stderr);
    } else if(!stderrfmtall.isEmpty()){
        fprintf(stderr, "%s", makeLog(job, stderrfmtall).cc());
        fflush(stderr);
    }

    // Do any file logging
    if(!job->stdio){
//        writemutex.lock();
        for(zu64 i = 0; i < logfiles.size(); ++i){
            formatmutex.lock();
            ZPath filename = logfiles.key(i);
            ZString filefmt = logfiles[i][job->source];
            ZString filefmtall = logfiles[i][ZLogSource::all];
            formatmutex.unlock();

            if(!filefmt.isEmpty()){
                ZFile::createDirsTo(filename);
//                ZFile out(filename, ZFile::modewrite | ZFile::append);
//                if(out.isOpen()){
//                    ZString logstr = makeLog(job, filefmt);
//                    out.write((const zbyte *)logstr.cc(), logstr.size());
//                } else {
//                    fprintf(stderr, "%s", "failed to open log file");
//                    fflush(stderr);
//                }
//                out.close();

                std::ofstream lgfl(filename.str().cc(), std::ios::app);
                lgfl << makeLog(job, filefmt);
                lgfl.flush();
                lgfl.close();
            } else if(!filefmtall.isEmpty()){
                ZFile::createDirsTo(filename);
//                ZFile out(filename, ZFile::modewrite | ZFile::append);
//                if(out.isOpen()){
//                    ZString logstr = makeLog(job, filefmtall);
//                    out.write((const zbyte *)logstr.cc(), logstr.size());
//                } else {
//                    fprintf(stderr, "%s", "failed to open log file");
//                    fflush(stderr);
//                }
//                out.close();

                std::ofstream lgfl(filename.str().cc(), std::ios::app);
                lgfl << makeLog(job, filefmtall);
                lgfl.flush();
                lgfl.close();
            }
        }
//        writemutex.unlock();
    }

    // Destroy the job
    delete job;
}

void ZLogWorker::formatStdout(zlog_source type, ZString fmt){
    formatmutex.lock();
    stdoutlog[type] = fmt;
    formatmutex.unlock();
}
void ZLogWorker::formatStderr(zlog_source type, ZString fmt){
    formatmutex.lock();
    stderrlog[type] = fmt;
    formatmutex.unlock();
}
void ZLogWorker::addLogFile(ZPath pth, zlog_source type, ZString fmt){
    formatmutex.lock();
    logfiles[pth][type] = fmt;
    formatmutex.unlock();
}

}
