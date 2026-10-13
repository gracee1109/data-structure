#include "common.hpp"
using namespace nccu;

int main() {
    string x="ACACACBAACCAA",y="CAACABCACA";
    auto d=lcsTable(x,y); assert(d[13][10]==8);
    for (const auto& row:d) printVector(row);
    cout<<"L[12,9] (character indices)="<<d[13][10]<<'\n';
    auto s=lcsString(x,y,d); assert(s=="AACACACA"); cout<<"LCS="<<s<<'\n';
}
