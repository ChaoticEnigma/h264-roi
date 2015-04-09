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

ZLog::ZLog(zlog_source source) : job(new LogJob), stdiolog(false), newline(true), rawlog(false), /*synclog(false),*/ noqueue(false){
    job->source = source;
    job->time = ZTime::now();
    job->clock = ZClock();
    job->thread = ZThread::thisTid();
}

ZLog::ZLog(ZLog::zlog_source source, ZString prefile, ZString preline, ZString prefunc) : ZLog(source){
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
    if(flag == flush){
        flushLog(false);
    } else if(flag == newln){
        log("\n");
    } else if(flag == flushln){
        log("\n");
        flushLog(false);
    } else if(flag == noln){
        newline = false;
    } else if(flag == raw){
        rawlog = true;
    } else if(flag == stdio){
        stdiolog = true;
    } else if(flag == this_thread){
        noqueue = true;
    }
    return *this;
}

ZLog &ZLog::log(ZString logtext){
    if(job)
        job->log.append(logtext);
    return *this;
}

ZLog &ZLog::operator<<(ZLogInfo in){
    if(job){
        switch(in.type){
            case ZLogInfo::file:
                job->file = in.info;
                break;
            case ZLogInfo::line:
                job->line = in.info;
                break;
            case ZLogInfo::function:
                job->func = in.info;
                break;

            case ZLogInfo::time:
    //            job->file = in.info;
                break;
            case ZLogInfo::clock:
    //            job->line = in.info;
                break;
            case ZLogInfo::thread:
    //            job->func = in.info;
                break;

            default:
                break;
        }
    }
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
    strftime(buffer, 20, "%m-%d-%y_%H-%M-%S", localtime(&raw));
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

void ZLog::formatStdout(zlog_source type, ZString fmt){
    ZLogWorker::formatStdout(type, fmt);
}
void ZLog::formatStderr(zlog_source type, ZString fmt){
    ZLogWorker::formatStderr(type, fmt);
}
void ZLog::addLogFile(ZPath pth, zlog_source type, ZString fmt){
    ZLogWorker::addLogFile(pth, type, fmt);
}

void ZLog::waitEnd(){
    worker->waitEnd();
}

}
