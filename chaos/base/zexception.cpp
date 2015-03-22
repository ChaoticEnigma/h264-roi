#include "zexception.h"
#include "zerror.h"
#include "zlog.h"

namespace LibChaos {

ZException::ZException(ZString str, int code, bool trace) : description(str), err(code){
    if(trace)
        stacktrace = ZError::getStackTrace(2);
}

ZString ZException::traceStr() const {
    ZString str = "**************************************\n";
    for(zu64 i = 0; i < stacktrace.size(); ++i){
        str += stacktrace[i] + "\n";
    }
    str += "**************************************";
    return str;
}

void ZException::logStackTrace() const {
    ELOG(ZLog::raw << trace() << ZLog::newln);
}

}
