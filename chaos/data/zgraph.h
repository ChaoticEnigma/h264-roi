#ifndef ZGRAPH_H
#define ZGRAPH_H

#include "ztypes.h"
#include "zmap.h"

namespace LibChaos {

template <typename T> class ZGraph {
public:
    struct Weight {
        zu64 weight;
    };

public:
    ZGraph(){

    }

private:
    //! Graph adjacency map.
    ZMap<T, ZMap<T, Weight> _adjmap;
};

}

#endif // ZGRAPH_H

