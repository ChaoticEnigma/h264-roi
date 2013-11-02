#include "zlogworker.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstdlib>

namespace LibChaos {

ZMutex< std::queue<LogJob> > ZLogWorker::jobs;
ZLogWorker::zlog_out ZLogWorker::stdoutlog;
ZLogWorker::zlog_out ZLogWorker::stderrlog;
ZArray<ZLogWorker::zlog_outfile> ZLogWorker::logfiles;
bool ZLogWorker::lastcomp;

ZLogWorker::ZLogWorker(){
    //work = work(zlogWorker);
    formatStdout(ZlogFormat(true, false, 0, false), ZlogFormat(false, false, 0, false), ZlogFormat(false, false, 0, false));
    formatStderr(ZlogFormat(false, false, 0, false), ZlogFormat(false, false, 0, false), ZlogFormat(true, false, 0, false));
}

ZLogWorker::~ZLogWorker(){
    //std::cout << "zlogworker" << std::endl;
    work.stop();
    work.join();
    //work.interrupt();
    //pthread_cancel(work);

}

int ZLogWorker::run(){
    signal(SIGABRT, sigHandle);
    signal(SIGFPE, sigHandle);
    signal(SIGILL, sigHandle);
    signal(SIGINT, sigHandle);
    signal(SIGSEGV, sigHandle);
    signal(SIGTERM, sigHandle);
    work.run(zlogWorker);
    //work = boost::thread(zlogWorker);
    //pthread_create(&work, NULL, zlogWorker, NULL);
    return 0;
}

void ZLogWorker::sigHandle(int sig){
    std::cout << "Caught signal " << sig << std::endl;
    exit(sig);
}

void ZLogWorker::wait(){
    if(!ZLogWorker::jobs.read().empty()){
        lastcomp = false;
    }
    while(!lastcomp){
        auto start = std::chrono::high_resolution_clock::now();
        auto end = start + std::chrono::microseconds(10000);
        do {
            std::this_thread::yield();
        } while (std::chrono::high_resolution_clock::now() < end);
    }
    //pthread_join(work, NULL);
    //work.join();
}

void ZLogWorker::queue(LogJob lj){
    jobs.lock();
    jobs.data().push(lj);
    jobs.unlock();
}

void *ZLogWorker::zlogWorker(void *zarg){
    ZThreadArg *arg = (ZThreadArg*)zarg;
    //std::cout << "worker work is working..." << std::endl;
    while(!arg->stop()){
        if(!jobs.read().empty()){
            jobs.lock();
            std::queue<LogJob> tmp;
            jobs.data().swap(tmp);
            jobs.unlock();

            while(!tmp.empty()){
                doLog(tmp.front());
                tmp.pop();
            }
            lastcomp = true;
        }
        // Yield for 10 ms
        std::this_thread::yield();
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return NULL;
}

ZString ZLogWorker::formatLog(LogJob jb, ZlogFormat fmt){
    ZString output;
    if(fmt.prefix && !jb.raw){
        if(fmt.time == 1){
            output << jb.time << " ";
        } else if(fmt.time == 2){
            output << jb.clock << " ";
        }
        if(fmt.thread)
            output << jb.thread << " ";
        if(fmt.time > 0 || fmt.thread)
            output << "- ";
    }
    output << jb.log;
    if(jb.newln)
        output << '\n';
    return output;
}

void ZLogWorker::doLog(LogJob jb){
    ZlogFormat outfmt, errfmt;

    switch(jb.source){
    case 0: // 0 - normal
        outfmt = stdoutlog.normal;
        errfmt = stderrlog.normal;
        if(!jb.stdout_this){
            for(unsigned i = 0; i < logfiles.size(); ++i){
                if(logfiles[i].normal.enable){
                    logfiles[i].file.createDirsTo();
                    std::ofstream lgfl(logfiles[i].file.str().cc(), std::ios::app);
                    lgfl << formatLog(jb, logfiles[i].normal);
                    lgfl.flush();
                    lgfl.close();
                }
            }
        }
        break;
    case 1: // 1 - debug
        outfmt = stdoutlog.debug;
        errfmt = stderrlog.debug;
        if(!jb.stdout_this){
            for(unsigned i = 0; i < logfiles.size(); ++i){
                if(logfiles[i].debug.enable){
                    logfiles[i].file.createDirsTo();
                    std::ofstream lgfl(logfiles[i].file.str().cc(), std::ios::app);
                    lgfl << formatLog(jb, logfiles[i].debug);
                    lgfl.flush();
                    lgfl.close();
                }
            }
        }
        break;
    case 2: // 2 - error
        outfmt = stdoutlog.error;
        errfmt = stderrlog.error;
        if(!jb.stdout_this){
            for(unsigned i = 0; i < logfiles.size(); ++i){
                if(logfiles[i].error.enable){
                    logfiles[i].file.createDirsTo();
                    std::ofstream lgfl(logfiles[i].file.str().cc(), std::ios::app);
                    lgfl << formatLog(jb, logfiles[i].error);
                    lgfl.flush();
                    lgfl.close();
                }
            }
        }
        break;
    }

    if(outfmt.enable){
        std::cout << formatLog(jb, outfmt) << std::flush;
    }
    if(errfmt.enable){
        std::cerr << formatLog(jb, errfmt) << std::flush;
    }
}

void ZLogWorker::formatStdout(ZlogFormat nml, ZlogFormat dbg, ZlogFormat err){
    if(nml._init)
        stdoutlog.normal = nml;
    if(dbg._init)
        stdoutlog.debug = dbg;
    if(err._init)
        stdoutlog.error = err;
}
void ZLogWorker::formatStderr(ZlogFormat nml, ZlogFormat dbg, ZlogFormat err){
    if(nml._init)
        stderrlog.normal = nml;
    if(dbg._init)
        stderrlog.debug = dbg;
    if(err._init)
       stderrlog.error = err;
}
void ZLogWorker::addLogFile(ZPath pth, ZlogFormat nml, ZlogFormat dbg, ZlogFormat err){
    zlog_outfile nwfl;
    if(!pth.str().isEmpty())
        nwfl.file = pth;
    if(nml._init)
        nwfl.normal = nml;
    if(dbg._init)
        nwfl.debug = dbg;
    if(err._init)
        nwfl.error = err;

    logfiles.push(nwfl);
}

}
