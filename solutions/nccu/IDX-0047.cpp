#include "common.hpp"
using namespace nccu;

int main() {
    auto tree=parseExpression("9*(3*5)-28/(8-1)<105+2*(3+4)-18");
    cout<<"tree: "; printTree(tree.get()); cout<<'\n';
    cout<<"prefix: "; printVector(traversal(tree,0)); cout<<"postfix: "; printVector(traversal(tree,2));
    assert(evaluate(tree.get())==0); cout<<"value="<<evaluate(tree.get())<<'\n';
    cout<<"parenthesized: "; printExpression(tree.get()); cout<<'\n';
}
