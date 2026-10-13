#include "common.hpp"
using namespace nccu;

int main() {
    auto trees=allTrees(3); assert(trees.size()==26);
    assert(set<string>(trees.begin(),trees.end()).size()==trees.size());
    assert(allTrees(0)==vector<string>({"."}));
    for (const auto& shape:trees) cout<<shape<<'\n';
    cout<<"count="<<trees.size()<<'\n';
}
