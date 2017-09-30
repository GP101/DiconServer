#include "KGen.h"
#include "KGlobalSingletonNotGood.h"

class KTest : public KGlobalSingleton<KTest>
{
public:
    KTest()
    {
        std::cout << "KTest" << std::endl;
    }
    void Test()
    {
        std::cout << "Test" << std::endl;
    }
};

#define _KTest          KTest::Singleton()

class KFoo
{
public:
    KFoo()
    {
        std::cout << "KFoo" << std::endl;
    }
};

KFoo    g_foo;

void main()
{
    std::cout << "Hello" << std::endl;
    _KTest.Test();
}
