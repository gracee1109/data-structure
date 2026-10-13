#include "common.hpp"
using namespace nccu;

int main() {
    auto tree=letterTree111(); auto pre=traversal(tree,0),post=traversal(tree,2);
    assert(pre==vector<char>({'+','*','-','*','A','B','C','D','E'}));
    assert(post==vector<char>({'A','B','*','C','-','D','*','E','+'}));
    cout<<"tree: "; printTree(tree.get()); cout<<'\n';
    cout<<"prefix: "; for (char c:pre) cout<<c; cout<<'\n';
    cout<<"postfix: "; for (char c:post) cout<<c; cout<<'\n';
}
