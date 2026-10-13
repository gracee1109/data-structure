#include "common.hpp"
using namespace nycu;

int main() {
    auto g=matrix(6,graph114(false));auto weighted=dijkstra(g,5),hops=bfs(g,5);
    auto shortest=restore(weighted,5,1),bfsPath=restore(hops,5,1);
    assert(shortest==vector<int>({5,4,1}) && weighted.distance[1]==2);
    assert(bfsPath==vector<int>({5,3,1}) && pathWeight(g,bfsPath)==3 && hops.distance[1]==2);
    cout<<"weighted shortest: ";printPath(shortest);cout<<"weight="<<weighted.distance[1]<<'\n';
    cout<<"BFS order: ";printPath(hops.order);cout<<"BFS path: ";printPath(bfsPath);
    cout<<"BFS hops="<<hops.distance[1]<<"; path weight="<<pathWeight(g,bfsPath)<<'\n';
}
