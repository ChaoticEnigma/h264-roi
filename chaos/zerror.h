#ifndef ZERROR_H
#define ZERROR_H

#include "zstring.h"

namespace LibChaos {
namespace ZError {

void registerSigSegv();

enum zerror_signal {
    unknown = 0,
    interrupt = 1,  // SIGINT
    abort = 2,      // SIGABRT
    quit = 3,       // SIGQUIT
    illegal = 4,    // SIGILL
    segv = 5,       // SIGSEGV
    terminate = 6,  // SIGTERM
    fpe = 7         // SIGFPE
};

typedef void (*signalHandler)(zerror_signal);

bool registerInterruptHandler(signalHandler);
bool registerSignalHandler(zerror_signal, signalHandler);

ZString getError();

}
}

#endif // ZERROR_H
