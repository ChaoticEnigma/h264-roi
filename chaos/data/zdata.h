/*******************************************************************************
**                                  LibChaos                                  **
**                                  zdata.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZDATA_H
#define ZDATA_H

#include <string.h>

namespace LibChaos {

struct ZData {
public:
    bool operator == (const ZData &rhs){
        return memcmp(this, &rhs, sizeof(*this));
    }
    bool operator!=(const ZData &rhs){
        return !operator==(rhs);
    }
};

}

#endif // ZDATA_H
