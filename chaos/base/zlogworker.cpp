#include "zlogworker.h"
#include "zfile.h"
#include "zqueue.h"

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

ZMutex writemutex;

ZMutex formatmutex;
ZMap<ZLogWorker::zlog_source, ZString> stdoutlog;
ZMap<ZLogWorker::zlog_source, ZString> stderrlog;
ZAssoc< ZPath, ZMap<ZLogWorker::zlog_source, ZString> > logfiles;

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
    // BUG: Sometimes hangs
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
                jobmutex.lock(); // Lock again before while(jobs.empty()) condition
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

ZString ZLogWorker::makeLog(LogJob *job, ZString fmt){
    if(!job->raw){
        fmt.replace("%log%", job->log);

        fmt.replace("%clock%", job->clock.getClock());
        fmt.replace("%date%", job->time.getDate());
        fmt.replace("%time%", job->time.getTime());
        fmt.replace("%datetime%", job->time.getDate() + " " + job->time.getTime());
        fmt.replace("%thread%", job->thread);

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

    // Do any stdout logging
    if(!stdoutlog[job->source].isEmpty()){
        fprintf(stdout, "%s", makeLog(job, stdoutlog[job->source]).cc());
        fflush(stdout);
    } else if(!stdoutlog[ZLogSource::all].isEmpty()){
        fprintf(stdout, "%s", makeLog(job, stdoutlog[ZLogSource::all]).cc());
        fflush(stdout);
    }

    // Do any stderr logging
    if(!stderrlog[job->source].isEmpty()){
        fprintf(stderr, "%s", makeLog(job, stderrlog[job->source]).cc());
        fflush(stderr);
    } else if(!stderrlog[ZLogSource::all].isEmpty()){
        fprintf(stderr, "%s", makeLog(job, stderrlog[ZLogSource::all]).cc());
        fflush(stderr);
    }

    // Do any file logging
    if(!job->stdio){
        writemutex.lock();
        for(zu64 i = 0; i < logfiles.size(); ++i){
            if(!logfiles[i][job->source].isEmpty()){
                ZFile::createDirsTo(logfiles.key(i));

                std::ofstream lgfl(logfiles.key(i).str().cc(), std::ios::app);
                lgfl << makeLog(job, logfiles[i][job->source]);
                lgfl.flush();
                lgfl.close();
            } else if(!logfiles[i][ZLogSource::all].isEmpty()){
                ZFile::createDirsTo(logfiles.key(i));

                std::ofstream lgfl(logfiles.key(i).str().cc(), std::ios::app);
                lgfl << makeLog(job, logfiles[i][ZLogSource::all]);
                lgfl.flush();
                lgfl.close();
            }
        }
        writemutex.unlock();
    }

    // Destroy the job
    delete job;

    formatmutex.unlock();
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
