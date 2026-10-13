#include "common.hpp"
using namespace nycu;

int main() {
    // 原題未提供指定圖；此圖用於驗證兩個DP定義的差別。
    Matrix g=matrix(4,{{0,1,2},{1,2,3},{0,2,9},{2,3,1},{0,3,20}});
    auto f0=boundedAPSP(g,0),f1=boundedAPSP(g,1),f3=boundedAPSP(g,3),all=floyd(g);
    assert(f0[0][3]==INF && f1[0][3]==20 && f3[0][3]==6 && f3==all);
    cout<<"f(i,j,n-1):\n";printMatrix(f3);
    cout<<"Floyd-Warshall:\n";printMatrix(all);
}
