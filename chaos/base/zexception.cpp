/*******************************************************************************
**                                  LibChaos                                  **
**                               zexception.cpp                               **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zexception.h"
#include "zerror.h"
#include "zlog.h"

namespace LibChaos {

// Constructor should not be able to throw an exception
ZException::ZException(ZString desc, int code, bool trace) : _description(desc), _error(code){
    if(trace)
        _stacktrace = ZError::getStackTrace(2);
}

ZString ZException::traceStr() const {
    ZString str = "**************************************\n";
    for(zu64 i = 0; i < _stacktrace.size(); ++i){
        str += _stacktrace[i] + "\n";
    }
    str += "**************************************";
    return str;
}

void ZException::logStackTrace() const {
    ELOG(ZLog::raw << trace() << ZLog::newln);
}

}
