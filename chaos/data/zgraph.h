#ifndef ZGRAPH_H
#define ZGRAPH_H

#include "ztypes.h"
#include "zlist.h"
#include "zmap.h"

#define ZGRAPH_EDGE_EXISTS 0x01

namespace LibChaos {

/*! Generic weighted, directed graph implementation.
 *  Effectively an unweighted graph when every edge has the same weight.
 *  Effectively an undirected graph when every edge has a complement edge.
 */
template <typename V, typename E> class ZGraph {
public:
    struct Edge {
        V parent;
        V child;
        E edge;
    };

public:
    //! Default constructor.
    ZGraph(){

    }

    //! Construct a graph with a list of \a vertices.
    ZGraph(ZList<V> vertices){
        ZMap<V, EdgeWeight> inner;
        // Add vertices to inner map
        for(auto i = vertices.begin(); i.more(); ++i){
            inner.add(*i, { 0, E() });
        }
        // Map each vertex to another identical map
        for(auto i = vertices.begin(); i.more(); ++i){
            _adjmap.add(*i, inner);
        }
    }

    //! Construct a graph with a list of \a vertices and a list of \a edges.
    ZGraph(ZList<V> vertices, ZList<Edge> edges) : ZGraph(vertices){
        // Connect edges
        for(auto i = edges.begin(); i.more(); ++i){
            connect(i.get().parent, i.get().child, i.get().edge);
        }
    }

    //! Add \a vertex to the graph.
    void add(V vertex){
        ZMap<V, EdgeWeight> inner;
        // Add vertices to inner map
        for(auto i = _adjmap.begin(); i.more(); ++i){
            inner.add(*i, { 0, E() });
        }
        // Add outer vertex with inner map
        _adjmap.add(vertex, inner);
        // Add inner vertices
        for(auto i = _adjmap.begin(); i.more(); ++i){
            _adjmap[*i].add(vertex, { 0, E() });
        }
    }

    //! Remove \a vertex from the graph.
    void remove(V vertex){
        // Remove outer vertx
        _adjmap.remove(vertex);
        // Remove innter vertices
        for(auto i = _adjmap.begin(); i.more(); ++i){
            _adjmap[*i].remove(vertex);
        }
    }

    //! Connect a \a parent vertex to \a child vertex with an \a edge.
    void connect(V parent, V child, E edge = E()){
        _adjmap[parent][child].flags = ZGRAPH_EDGE_EXISTS;
        _adjmap[parent][child].edge = edge;
    }

    //! Determine if two vertices are connected.
    bool connected(V parent, V child) const {
        return (_adjmap.contains(parent) && _adjmap.contains(child) && _adjmap[parent][child].flags == ZGRAPH_EDGE_EXISTS);
    }

    /*! Get the weight of the edge between two vertices.
     *  \throws ZException Vertices are not connected.
     */
    E weight(V parent, V child, E edge) const {
        if(!connected(parent, child))
            throw ZException("Vertices are not connected");
        return _adjmap[parent][child].edge;
    }

    //! Get an array of the vertices in the graph.
    ZArray<V> vertices() const {
        ZArray<V> verts;
        for(auto i = _adjmap.begin(); i.more(); ++i){
            verts.push(*i);
        }
        return verts;
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

