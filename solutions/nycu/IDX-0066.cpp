#include "common.hpp"
using namespace nycu;

int main() {
    auto answers=logic8(0b10101010,0b11110000);
    assert((answers==array<uint8_t,3>{0b10100000,0b11111010,0b01011010}));
    array<string,3> names{"AND","OR","XOR"};
    for (int i=0;i<3;++i) cout<<names[i]<<'='<<bitset<8>(answers[i])<<'\n';
}
