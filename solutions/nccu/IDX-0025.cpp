#include "common.hpp"
using namespace nccu;

int main() {
    auto tree=companyTree(); auto out=traversal(tree,0);
    assert(out==companyPre); printVector(out);
}
