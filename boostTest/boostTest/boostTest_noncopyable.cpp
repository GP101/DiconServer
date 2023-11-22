#include "KGen.h"
#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include "noncopyable.h"

class KTest : public noncopyable
{
public:
    KTest()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
private:
    //KTest( const KTest& rhs );
};

void main()
{
    KTest t;
    //std::vector<KTest>  v;
    //for (int i = 0; i < 100; ++i)
    //{
    //    v.push_back( KTest());
    //}
}
