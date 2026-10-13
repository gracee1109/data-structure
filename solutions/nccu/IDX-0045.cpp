#include "common.hpp"
using namespace nccu;

int main() {
    Ptr<int> root;
    for (int key:keys112) {
        avlInsert(root,key,less<int>{},&cout);
        cout<<"insert "<<key<<": "; printTree(root.get()); cout<<'\n'; checkAVL(root.get());
    }
    auto expected=keys112; sort(expected.begin(),expected.end()); assert(traversal(root,1)==expected);
    cout<<"inorder: "; printVector(traversal(root,1)); assert(root->key==72);
    bstInsert(root,118); root=splay(move(root),118); expected.insert(lower_bound(expected.begin(),expected.end(),118),118);
    assert(root->key==118 && traversal(root,1)==expected);
    cout<<"splay put(118): "; printTree(root.get()); cout<<'\n';
}
