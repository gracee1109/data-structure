#include "common.hpp"
using namespace nccu;

int main() {
    assert(pascal(1)==vector<U64>({1})); assert(pascal(3)==vector<U64>({1,2,1}));
    assert(pascal(4)==vector<U64>({1,3,3,1})); printVector(pascal(4));
}
