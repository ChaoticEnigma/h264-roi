/*******************************************************************************
**                                  LibChaos                                  **
**                                zexception.h                                **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZEXCEPTION_H
#define ZEXCEPTION_H

#include "zarray.h"
#include "zstring.h"

namespace LibChaos {

class ZException {
public:
    ZException(ZString description = ZString(), int code = 0, bool trace = true);

    inline const ZString &what() const { return description; }
    inline int code() const { return err; }

    inline const ArZ &trace() const { return stacktrace; }
    ZString traceStr() const;

    void logStackTrace() const;

private:
    ZString description;
    int err;
    ArZ stacktrace;
};

}

#endif // ZEXCEPTION_H
