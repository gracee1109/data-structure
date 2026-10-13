#include "common.hpp"
using namespace nccu;

int main() {
    assert(fibonacci(0)==0 && fibonacci(1)==1 && fibonacci(10)==55);
    assert(fibonacci(93)==12200160415121876738ULL);
    cout<<"F(10)="<<fibonacci(10)<<'\n';
}
