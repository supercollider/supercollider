#include "source/utilities/rng.hpp"
#include <iostream>
using namespace std;

int main()
{
    PNPD::FastRng rng;

    for (int i = 0; i != 100000000; ++i)
    {
        /* cout <<  */rng.GenInt() /* <<endl */;
    }
    cout<< rng.GenFloat() <<endl;
    return 0;
}
