#include "common.hpp"
using namespace nycu;

int main() {
    U64 comparisons=0;auto merged=mergeLists(vector<int>{1,3,5},vector<int>{2,4,6},less<int>{},&comparisons);
    assert(merged==vector<int>({1,2,3,4,5,6}) && comparisons==5);
    cout<<"merge: ";printVector(merged);cout<<"comparisons="<<comparisons<<'\n';
    vector<int> a{9,1,4,3,2,8,6,5,7},expected=a;sort(expected.begin(),expected.end());
    mergeSort(a,less<int>{},&cout);assert(a==expected);cout<<"sorted: ";printVector(a);
}
