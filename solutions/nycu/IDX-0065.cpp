#include "common.hpp"
using namespace nycu;

int main() {
    auto edges=graph114(true);auto g=matrix(6,edges);
    auto wrong=frozenDijkstraForCounterexample(g,5),right=bellmanFord(6,edges,5);
    assert(wrong.distance[1]==2 && right.distance[1]==1);
    assert(restore(right,5,1)==vector<int>({5,3,2,1}));
    cout<<"counterexample Dijkstra path: ";printPath(restore(wrong,5,1));cout<<"wrong distance="<<wrong.distance[1]<<'\n';
    cout<<"Bellman-Ford path: ";printPath(restore(right,5,1));cout<<"true distance="<<right.distance[1]<<'\n';
    assert(floyd(g)[5][1]==1);
    bool rejected=false;try {(void)dijkstra(g,5);}catch (const invalid_argument&) {rejected=true;}
    assert(rejected);cout<<"general-purpose Dijkstra rejects the negative-edge input\n";
}
