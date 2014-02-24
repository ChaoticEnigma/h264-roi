#ifndef ZERROR_H
#define ZERROR_H

namespace LibChaos {

namespace ZError {

void registerSigSegv();
bool registerExitHandler();

}

}

#endif // ZERROR_H
