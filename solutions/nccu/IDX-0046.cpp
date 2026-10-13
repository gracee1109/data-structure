#include "common.hpp"
using namespace nccu;

int main() {
    for (bool doubleHash: {false,true}) {
        cout<<(doubleHash?"DOUBLE\n":"LINEAR\n"); vector<optional<int>> table(27);
        for (int k:keys112) hashInsert(table,k,k%27,doubleHash?13-k%13:1,&cout);
        assert(count_if(table.begin(),table.end(),[](const auto& x){return x.has_value();})==16);
        printHash(table);
    }
}
