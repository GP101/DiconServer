#include <stdio.h>
#include <algorithm>
#include <functional>
#include <vector>

struct void_;

template<int _Nx>
class _Ph // placeholder
{
};

typedef _Ph<1>  _1;
typedef _Ph<2>  _2;
typedef _Ph<3>  _3;

template<typename Selecter, typename A = void_, typename B = void_>
struct KTypeSelecter
{
    typedef void_ type;
};//struct KTypeSelecter

template<typename A, typename B>
struct KTypeSelecter<_1,A,B>
{
    typedef A type;
};//struct KTypeSelecter

template<typename A, typename B>
struct KTypeSelecter<_2,A,B>
{
    typedef B type;
};//struct KTypeSelecter

template<class Selecter>
struct KValueSelector
{
    void operator()()
    {
    }//operator()()
};//struct KValueSelector

template<>
struct KValueSelector<_1>
{
    template<typename C>
    C& operator()(C& first)
    {
        return first;
    }//operator()()

    template<typename C, typename D>
    C& operator()(C& first, D& second)
    {
        return first;
    }//operator()()
};//struct KValueSelector

template<>
struct KValueSelector<_2>
{
    template<typename C, typename D>
    D& operator()(C& first, D& second)
    {
        return second;
    }//operator()()
};//struct KValueSelector

void main()
{
    // _2 select second type 'float'
    KTypeSelecter<_2, int, float>::type   t1;
    t1 = 1.1f;
    KTypeSelecter<_1, int>::type         t2;
    t2 = 2;

    printf("%g\r\n", t1);
    printf("%i\r\n", t2);

    int a = 0;
    float b = 0.f;
    float c = 0.f;

    KValueSelector<_1>()(a, b) = 3;
    KValueSelector<_2>()(a, b) = 4.3f;
    KValueSelector<_1>()(c) = 5.4f;

    printf("%i, %g, %g\r\n", a, b, c);
}//main()
