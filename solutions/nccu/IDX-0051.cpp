#include "common.hpp"
using namespace nccu;

int main() {
    vector<int> a{5,1,4,2,3},b=a; selectionSort(a); mergeSort(b);
    assert(a==vector<int>({1,2,3,4,5}) && a==b); printVector(a);
    cout<<"choice=A; selection Theta(n^2), merge Theta(n log n)\n";
}
