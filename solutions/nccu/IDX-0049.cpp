#include "common.hpp"
using namespace nccu;

int main() {
    auto tree=letterTree112(); auto pre=traversal(tree,0),post=traversal(tree,2);
    assert(pre==vector<char>({'E','H','D','B','F','G','A'}));
    assert(post==vector<char>({'D','F','B','H','A','G','E'}));
    cout<<"prefix: "; for (char c:pre) cout<<c; cout<<'\n';
    cout<<"postfix: "; for (char c:post) cout<<c; cout<<"\nchoice=D\n";
}
