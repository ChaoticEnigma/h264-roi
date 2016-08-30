/*******************************************************************************
**                                  LibChaos                                  **
**                                  zlog.cpp                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zlog.h"
#include <ctime>
#include <cstdio>

#if COMPILER == MSVC
    #include <windows.h>
#endif

namespace LibChaos {

std::atomic<bool> ZLog::_init(false);
ZLogWorker *ZLog::worker = nullptr;

ZLog::ZLog(zlog_level source) : job(new LogJob), stdiolog(false), newline(true), rawlog(false), /*synclog(false),*/ noqueue(false){
    job->level = source;
    job->time = ZTime::now();
    job->clock = ZClock();
    job->thread = ZThread::thisTid();
}

ZLog::ZLog(zlog_level source, ZString prefile, ZString preline, ZString prefunc) : ZLog(source){
    job->file = prefile;
    job->line = preline;
    job->func = prefunc;
}

ZLog::~ZLog(){
    if(job && !job->log.isEmpty()){
        flushLog(true);
    }
}

void ZLog::flushLog(bool final){
    if(!job)
        return;

    job->stdio = stdiolog;
    job->newln = newline;
    job->raw = rawlog;
    //job->log = buffer;

    // Transfer job ownership
    LogJob *jobptr = job;
    job = nullptr;
    if(!final){
        job = new LogJob(*jobptr);
    }

    if(!noqueue && _init){
        worker->queue(jobptr);
    } else {
        ZLogWorker::doLog(jobptr);
    }
}

ZLog &ZLog::operator<<(zlog_flag flag){
    if(flag == FLUSH){
        flushLog(false);
    } else if(flag == NEWLN){
        log("\n");
    } else if(flag == FLUSHLN){
        log("\n");
        flushLog(false);
    } else if(flag == NOLN){
        newline = false;
    } else if(flag == RAW){
        rawlog = true;
    } else if(flag == STDIO){
        stdiolog = true;
    } else if(flag == THIS_THREAD){
        noqueue = true;
    }
    return *this;
}

ZLog &ZLog::log(ZString logtext){
    if(job)
        job->log.append(logtext);
    return *this;
}

ZLog &ZLog::operator<<(ZBinary bin){
    ZString text;
    for(zu64 i = 0; i < bin.size(); ++i){
        unsigned char tmp = bin[i];
        // Log in hexadecimal pairs
        text += ZString::ItoS(tmp, 16, 2);
    }
    return log(text);
}

ZString ZLog::pullBuffer(){
    if(!job)
        return ZString();
    ZString tmp = job->log;
    job->log.clear();
    return tmp;
}

ZString ZLog::genLogFileName(ZString prefix){
    time_t raw;
    char buffer[20];
    time(&raw);
    strftime(buffer, 20, "%Y-%m-%d_%H.%M.%S", localtime(&raw));
    ZString out(prefix);
    out << buffer << ".log";
    return out;
}

void ZLog::init(){
    if(!_init){
        worker = new ZLogWorker;
        worker->run();
        _init = true;
    }
}

void ZLog::deInit(){
    if(_init){
        _init = false;
        worker->stop();
        delete worker;
    }
}

}
