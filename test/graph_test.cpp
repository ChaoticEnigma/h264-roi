#include "test.h"
#include "zgraph.h"

namespace LibChaosTest {

void graph(){
    ZList<ZString> verts1 = {"A","B","C","D","E"};
    ZGraph<ZString, int> graph1(verts1);

}

ZArray<Test> graph_tests(){
    return {
        { "graph", graph, true, {} },
    };
}

}
