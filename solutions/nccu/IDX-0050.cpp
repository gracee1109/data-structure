#include "common.hpp"
using namespace nccu;

int main() {
    auto [maximum,minimum]=avlNodeBounds(4); assert(maximum==31 && minimum==12);
    cout<<"edge-height 4: max="<<maximum<<" min="<<minimum<<" choice=C\n";
}
