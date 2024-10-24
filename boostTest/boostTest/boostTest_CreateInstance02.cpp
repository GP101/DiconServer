#include "KGen.h"
#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include "KBaseSingleton.h"


class KBaseLayer : public KBaseSingleton<KBaseLayer>
{
public:
    void Test()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

#define _KBaseLayer     (KBaseLayer::Instance())


class KMyLayer : public KBaseLayer
{
public:
    void Test2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

#define _KMyLayer     (std::static_pointer_cast<KMyLayer,KBaseLayer>(KBaseLayer::Instance()))


void main()
{
    KBaseLayer::CreateInstance<KMyLayer>(0);

    _KBaseLayer->Test();
    _KMyLayer->Test2();
}
