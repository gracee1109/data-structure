#include "common.hpp"
using namespace nccu;

int main() {
    vector<string> heap;
    for (const auto& s:companyPre) {
        heapPush(heap,s,StringLess{}); cout<<"insert "<<s<<": "; printVector(heap);
    }
    assert(heap==vector<string>({"US","Asia","Sales","Laptops","International","Canada","Manufacturing","Computers\"R\"Us","Desktops"}));
}
