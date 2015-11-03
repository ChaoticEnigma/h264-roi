#ifndef ZGRAPH_H
#define ZGRAPH_H

#include "ztypes.h"
#include "zlist.h"
#include "zmap.h"

namespace LibChaos {

/*! Generic weighted, directed graph implementation.
 *  Can emulate an unweighted graph when every edge has the same weight.
 *  Can emulate an undirected graph when every edge has a complement edge.
 */
template <typename V, typename E> class ZGraph {
public:
    struct Edge {
        V parent;
        V child;
        E edge;
    };

public:
    ZGraph(){

    }

    ZGraph(ZList<V> vertices){
        ZMap<V, EdgeWeight> inner;
        for(auto i = vertices.begin(); i.more(); ++i){
            inner.add(*i, { 0, E() });
        }
        for(auto i = vertices.begin(); i.more(); ++i){
            _adjmap.add(*i, inner);
        }
    }

    ZGraph(ZList<V> vertices, ZList<Edge> edges) : ZGraph(vertices){
        for(auto i = edges.begin(); i.more(); ++i){
            connect(i.get().parent, i.get().child, i.get().edge);
        }
    }

    void connect(V parent, V child, E edge){
        _adjmap[parent][child] = edge;
    }

private:
    struct EdgeWeight {
        zbyte flags;
        E edge;
    };

private:
    //! Graph adjacency map.
    ZMap<V, ZMap<V, EdgeWeight>> _adjmap;
};

}

#endif // ZGRAPH_H

