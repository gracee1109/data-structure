#include "common.hpp"
using namespace nccu;

int main() {
    WeightMatrix g{{0,1,INF,4},{1,0,2,INF},{INF,2,0,1},{4,INF,1,0}};
    auto d=dijkstraMatrix(g,0); assert(d==vector<long long>({0,1,3,4})); printVector(d);
    vector<vector<int>> adjacent{{1,3},{0,2},{1,3},{0,2}};
    auto messages=floodingMessages(adjacent); assert(messages==32); cout<<"flooding messages="<<messages<<"; answer=T under link-state model\n";
}
