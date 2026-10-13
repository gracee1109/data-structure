#include "common.hpp"
using namespace nccu;

int main() {
    // 原題未指定圖；這是自建驗證圖，非原卷圖形。
    Matrix g(5,vector<bool>(5));
    for (auto [u,v]:vector<pair<int,int>>{{0,1},{1,2},{2,3},{3,4},{0,4},{1,3}}) g[u][v]=g[v][u]=true;
    auto path=longestPath(g); assert(path.size()==5);
    for (int& v:path) { ++v; } // 還原為原題的1-based編號。
    printVector(path);
    Matrix disconnected(2,vector<bool>(2)); assert(longestPath(disconnected).empty());
    assert(longestPath(Matrix{{false}})==vector<int>({0}));
}
