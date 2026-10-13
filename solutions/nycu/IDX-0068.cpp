#include "common.hpp"
using namespace nycu;

int main() {
    U64 checked=0;auto found=fourCycleFast(sampleFourCycle(true),&checked);
    assert(found && !fourCycleFast(sampleFourCycle(false)));
    cout<<"4-cycle: ";printCycle(*found);cout<<"neighbor-pair checks="<<checked<<'\n';
    Graph star(6,vector<bool>(6));for (int i=1;i<6;++i) star[0][i]=star[i][0]=true;
    U64 checks=0;assert(!fourCycleFast(star,&checks) && checks==10);
    cout<<"6-vertex star: no 4-cycle; pair checks="<<checks<<'\n';
}
