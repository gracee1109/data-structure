#include "common.hpp"
using namespace nccu;

int main() {
    Ptr<string> root;
    for (const auto& s:companyIn) {
        avlInsert(root,s,StringLess{},&cout);
        cout<<"insert "<<s<<": "; printTree(root.get()); cout<<'\n'; checkAVL(root.get(),StringLess{});
    }
    auto expected=companyIn; sort(expected.begin(),expected.end(),StringLess{});
    assert(traversal(root,1)==expected); assert(root->key=="Canada");
}
