#include "common.hpp"
using namespace nccu;

int main() {
    WeightMatrix g{{0,2,5},{INF,0,1},{INF,INF,0}};
    auto d=dijkstraMatrix(g,0); assert(d==vector<long long>({0,2,3})); printVector(d);
    // S->A=2、S->B=5、B->A=-10：先定案 A=2 會漏掉真正距離 -5。
    g[1][2]=INF; g[2][1]=-10; bool rejected=false;
    try { (void)dijkstraMatrix(g,0); } catch (const invalid_argument&) { rejected=true; }
    assert(rejected); cout<<"negative-edge counterexample: true distance(S,A)=-5\n";
}
