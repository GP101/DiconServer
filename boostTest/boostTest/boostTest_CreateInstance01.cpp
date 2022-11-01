#include "KGen.h"
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <algorithm>
#include <iostream>


class KBaseLayer
{
public:
    template<typename T>
    static void CreateInstance( DWORD dwParam )
    {
        ms_instance.reset( new T() );
    }
protected:
    static boost::shared_ptr<KBaseLayer>    ms_instance;
public:
    static boost::shared_ptr<KBaseLayer> Instance()
    {
        return ms_instance;
    }

    void Test() 
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

boost::shared_ptr<KBaseLayer> KBaseLayer::ms_instance = nullptr;
#define _KBaseLayer     (KBaseLayer::Instance())

class KMyLayer : public KBaseLayer
{
public:
    void Test2() 
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

#define _KMyLayer     (boost::static_pointer_cast<KMyLayer,KBaseLayer>(KBaseLayer::Instance()))

void main()
{
    KBaseLayer::CreateInstance<KMyLayer>(0);

    _KBaseLayer->Test();
    _KMyLayer->Test2();
}
