#ifndef ZGRAPH_H
#define ZGRAPH_H

#include "ztypes.h"
#include "zmap.h"

namespace LibChaos {

/*! Generic weighted, directed graph implementation.
 *  Can emulate an unweighted graph when every edge has the same weight.
 *  Can emulate an undirected graph when every edge has a complement edge.
 */
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
    ZMap<T, ZMap<T, Weight>> _adjmap;
};

}

#endif // ZGRAPH_H

