#include "common.hpp"
using namespace nccu;

int main() {
    vector<int> unordered{8,2,7,1,4}; assert(sequential(unordered,7)==2 && sequential(unordered,9)==-1);
    auto ordered=unordered; sort(ordered.begin(),ordered.end()); assert(binary(ordered,7)==3 && binary(ordered,9)==-1);
    cout<<"choice=c; unordered: sequential; ordered: binary\n";
}
