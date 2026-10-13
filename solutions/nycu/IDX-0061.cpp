#include "common.hpp"
using namespace nycu;

int main() {
    auto g=matrix(6,mst115,true);auto from5=prim(g,4),from3=prim(g,2);
    assert(from5 && from3 && from5->weight==15 && from3->weight==15);
    vector<Cost> w5,w3;for (auto e:from5->edges) w5.push_back(e.w);for (auto e:from3->edges) w3.push_back(e.w);
    assert(w5==vector<Cost>({4,1,2,3,5}) && w3==vector<Cost>({1,2,3,4,5}));
    assert(edgeSet(*from5)==edgeSet(*from3));
    printMST(*from5,4);printMST(*from3,2);cout<<"same MST=true\n";
}
