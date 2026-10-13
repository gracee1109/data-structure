#include "common.hpp"
using namespace nccu;

int main() {
    auto tree=companyTree(); auto out=traversal(tree,2);
    assert(out==companyPost); printVector(out);
}
