#include "common.hpp"
using namespace nccu;

int main() {
    // 將原圖的 K,I,M,H,J,L 依序編成 0..5。
    vector<vector<int>> children{{1,2},{3,4},{5},{},{},{}};
    vector<char> names{'K','I','M','H','J','L'}; auto order=bfs(children,0);
    assert(order==vector<int>({0,1,2,3,4,5}));
    for (int i:order) { cout<<names[i]; }
    cout<<"\nanswer=F\n";
}
