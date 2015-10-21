/*******************************************************************************
**                                  LibChaos                                  **
**                                zexception.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZEXCEPTION_H
#define ZEXCEPTION_H

#include "zarray.h"
#include "zstring.h"

namespace LibChaos {

/*! Throwable Runtime Error.
 *  Exceptions are a widely used (and arguably overused) error-handling technique.
 *  In LibChaos, exceptions are only thrown on fatal, avoidable misuse of LibChaos
 *  classes and functions, where continuing would put the user or environment
 *  in an undefined or unreasonable state. For example, in an array of 5 integers,
 *  if the user tries to do array[10] = 255, there is no reasonable way for operator[]
 *  to return a reference to the user that would not result in an illegal memory access.
 *  Exceptions are a safe way to return control to the user, allowing a program to reover,
 *  rather than simply ending the program.
 */
class ZException {
public:
    ZException(ZString description = ZString(), int code = 0, bool trace = true);

    //! Get exception description.
    inline const ZString &what() const { return _description; }
    inline int code() const { return _error; }

    /*! Get exception stack trace (if supported).
     *  \note Not supported on Windows.
     */
    inline const ArZ &trace() const { return _stacktrace; }
    ZString traceStr() const;

    //! Log the stack trace with ZLog.
    void logStackTrace() const;

private:
    ZString _description;
    int _error;
    ArZ _stacktrace;
};

}

#endif // ZEXCEPTION_H
