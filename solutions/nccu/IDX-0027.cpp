#include "common.hpp"
using namespace nccu;

int main() {
    auto tree=companyTree(); auto out=traversal(tree,1);
    assert(out==companyIn); printVector(out);
}
