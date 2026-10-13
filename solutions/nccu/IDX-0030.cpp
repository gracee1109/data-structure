#include "common.hpp"
using namespace nccu;

int main() {
    vector<optional<string>> table(13);
    for (const auto& s:companyPost) {
        int code=0; for (char c:s) code+=ord(c);
        hashInsert(table,s,code%13,11-code%11,&cout);
    }
    assert(table[0]=="Computers\"R\"Us" && table[6]=="Sales" && table[7]=="Manufacturing");
    printHash(table);
}
