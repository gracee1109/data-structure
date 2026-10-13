#include "common.hpp"
using namespace nycu;

int main() {
    auto [ways,calls]=stairsRecursive(3);assert(ways==4 && calls==13 && stairsDP(3)==ways);
    cout<<"n=3 recursive="<<ways<<" calls="<<calls<<" DP="<<stairsDP(3)<<'\n';
    for (int n=0;n<=15;++n) assert(stairsRecursive(n).first==stairsDP(n));
    assert(stairsDP(73)==12903063846126135669ULL);
    cout<<"both algorithms agree for n=0..15\n";
}
