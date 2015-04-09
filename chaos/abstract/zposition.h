/*******************************************************************************
**                                  LibChaos                                  **
**                                 zposition.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZPOSITION_H
#define ZPOSITION_H

#include "ztypes.h"

namespace LibChaos {

class ZPosition {
public:
    virtual ~ZPosition(){}

    // Get the internal position of a persistent position index
    virtual zu64 getPos() const = 0;
    // Try to set the position of an internal persistent position index, return the new position
    virtual zu64 setPos(zu64 pos) = 0;
    // Check if the position is at the "end" of the stream or container
    virtual bool atEnd() const = 0;
    // Return position to beginning of stream or container, equivalent to setPos(0), return the new position
    virtual zu64 rewind(){ return setPos(0); }
};

}

#endif // ZPOSITION_H
