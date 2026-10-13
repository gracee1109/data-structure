#include "common.hpp"
using namespace nycu;

int main() {
    assert(smallSubsets(0)==1 && smallSubsets(8)==163);
    cout<<"(a) subsets(n=8)="<<smallSubsets(8)<<'\n';
    auto tree=recurrenceTree(8);assert(tree.work==715 && tree.calls==400);
    cout<<"(b) log2(7)="<<log2(7.0)<<"; representative T(8)="<<tree.work<<"; calls="<<tree.calls<<'\n';
    Chaining chain(5);LinearHash linear(5);
    for (int key:{1,6,11,16,21,26}) {
        chain.insert(key);bool placed=linear.insert(key);assert(placed==(key!=26));
    }
    assert(chain.contains(26) && !linear.contains(26));
    cout<<"(c) separate chaining:\n";chain.print();cout<<"linear probing:\n";linear.print();
}
