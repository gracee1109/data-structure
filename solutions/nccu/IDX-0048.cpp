#include "common.hpp"
using namespace nccu;

int main() {
    assert(balancedSplit("RLRRLLRLRL")==4); assert(balancedSplit("RLRRRLLRLL")==2);
    assert(balancedSplit("LLLRRR")==1 && balancedSplit("")==0);
    bool rejected=false; try { (void)balancedSplit("LLLRRRR"); } catch (const invalid_argument&) { rejected=true; }
    assert(rejected); cout<<"examples: 4, 2, 1 (third example corrected); invalid original rejected\n";
}
