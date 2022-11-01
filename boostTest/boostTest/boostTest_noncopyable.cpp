#include "KGen.h"
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

class KTest : public boost::noncopyable
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
