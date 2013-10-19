#include "zlog.h"
#include <ctime>
#include <cstdio>

namespace LibChaos {

#define ZLOG_FLUSH              1001
#define ZLOG_NEWLN              1002
#define ZLOG_FLUSHLN            1003
#define ZLOG_NOLN               1004
#define ZLOG_NORMAL             3001
#define ZLOG_DEBUG              3002
#define ZLOG_ERROR              5001
#define ZLOG_STDOUT             6001

//ZLog::zlog_flag::zlog_flag(short dat) : data(dat){}
//void ZLog::zlog_flag::operator=(short dat){
//    data = dat;
//}
//bool ZLog::zlog_flag::operator==(zlog_flag rhs){
//    if(rhs.data == data) return true; return false;
//}

//ZLog::zlog_flag ZLog::flush = ZLOG_FLUSH;
//ZLog::zlog_flag ZLog::newln = ZLOG_NEWLN;
//ZLog::zlog_flag ZLog::flushln = ZLOG_FLUSHLN;
//ZLog::zlog_flag ZLog::noln = ZLOG_NOLN;
//ZLog::zlog_flag ZLog::normal = ZLOG_NORMAL;
//ZLog::zlog_flag ZLog::debug = ZLOG_DEBUG;
//ZLog::zlog_flag ZLog::error = ZLOG_ERROR;
//ZLog::zlog_flag ZLog::stdio = ZLOG_STDOUT;

bool ZLog::_init = false;
ZLogWorker ZLog::worker;
AsArZ ZLog::thread_ids;

ZLog::ZLog() : source_mode(0), stdout_this(false), write_on_destruct(false), newline(true), rawlog(false){}

ZLog::~ZLog(){
    if(write_on_destruct)
        flushLog();
}

void ZLog::flushLog(){
    LogJob out;
    out.source = source_mode;
    out.clock = getClock();
    out.time = getTime();
    out.thread = getThread();
    out.log = buffer;
    out.stdout_this = stdout_this;
    out.newln = newline;
    out.raw = rawlog;
    if(_init)
        worker.queue(out);
    else
        ZLogWorker::doLog(out);
}

ZLog &ZLog::operator<<(zlog_flags flag){
    if(flag == flush){
        flushLog();
    } else if(flag == newln){
        log("\n");
    } else if(flag == flushln){
        log("\n");
        flushLog();
    } else if(flag == noln){
        newline = false;
    } else if(flag == raw){
        rawlog = true;
    } else if(flag == normal){
        source_mode = 0;
    } else if(flag == debug){
        source_mode = 1;
    } else if(flag == error){
        source_mode = 2;
    } else if(flag == stdio){
        stdout_this = true;
    }
    return *this;
}

ZLog &ZLog::log(ZString logtext){
    buffer << logtext;
    write_on_destruct = true;
    return *this;
}
ZLog &ZLog::operator<<(ZString text){
    return log(text);
}
ZLog &ZLog::operator<<(std::string text){
    return log(ZString(text));
}
ZLog &ZLog::operator<<(zs64 num){
    return log(ZString(num));
}
ZLog &ZLog::operator<<(const char *text){
    return log(ZString(text));
}
ZLog &ZLog::operator<<(char *text){
    return log(ZString(text));
}
ZLog &ZLog::operator<<(ZPath text){
    return log(text.str());
}
ZLog &ZLog::operator<<(ZBinary bin){
    ZString text;
    for(zu64 i = 0; i < bin.size(); ++i)
        text << bin[i];
    return log(text);
}

ZString ZLog::pullBuffer(){
    ZString tmp = buffer;
    buffer.clear();
    write_on_destruct = false;
    return tmp;
}

ZString ZLog::getTime(){
    time_t raw;
    time(&raw);
    struct tm *time;
    time = localtime(&raw);
    int mon = time->tm_mon + 1;
    int day = time->tm_mday;
    int yr = time->tm_year - 100;
    int hr = time->tm_hour;
    int min = time->tm_min;
    int sec = time->tm_sec;
    char buffer[20];
    sprintf(buffer, "%02d/%02d/%02d %02d:%02d:%02d", mon, day, yr, hr, min, sec);
    //strftime(buffer, 20, "%02m/%02d/%02y %02H:%02M:%02S", localtime(&raw));
    ZString out(buffer);
    return out;
}

ZString ZLog::getClock(){
    clock_t raw = clock();
    float rawsecs = (float)raw / (float)CLOCKS_PER_SEC;
    int secs = rawsecs;
    int msecs = (rawsecs - (float)secs) * 1000;
    int mins = secs / 60;
    secs = secs - (mins * 60);
    int hrs = mins / 60;
    mins = mins - (hrs * 60);
    char buffer[20];
    sprintf(buffer, "%02d:%02d:%02d:%03d", hrs, mins, secs, msecs);
    ZString out(buffer);
    return out;
}

ZString ZLog::getThread(){
    ZString thread = ZThreadA::thisTid();
    unsigned id;
    bool found = false;
    for(unsigned i = 0; i < thread_ids.size(); ++i){
        if(thread_ids[i] == thread){
            id = i;
            found = true;
            break;
        }
    }
    if(!found){
        id = thread_ids.size();
        thread_ids.push(thread);
    }
    return id;
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
        _init = true;
        worker.run();
    }
}
void ZLog::init(ZPath dlgfl){
    if(!_init){
        init();
        addLogFile(dlgfl, ZlogFormat(true, false, 0, false), ZlogFormat(true, false, 0, false), ZlogFormat(true, false, 0, false));
    }
}

void ZLog::formatStdout(ZlogFormat nml, ZlogFormat dbg, ZlogFormat err){
    ZLogWorker::formatStdout(nml, dbg, err);
}
void ZLog::formatStderr(ZlogFormat nml, ZlogFormat dbg, ZlogFormat err){
    ZLogWorker::formatStderr(nml, dbg, err);
}
void ZLog::addLogFile(ZPath pth, ZlogFormat nml, ZlogFormat dbg, ZlogFormat err){
    ZLogWorker::addLogFile(pth, nml, dbg, err);
}

}
