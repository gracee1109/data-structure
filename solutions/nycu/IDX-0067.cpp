#include "common.hpp"
using namespace nycu;

int main() {
    U64 checked=0;auto found=fourCycleBrute(sampleFourCycle(true),&checked);
    assert(found && checked==1 && !fourCycleBrute(sampleFourCycle(false)));
    cout<<"4-cycle: ";printCycle(*found);cout<<"checked quadruples="<<checked<<'\n';
}
