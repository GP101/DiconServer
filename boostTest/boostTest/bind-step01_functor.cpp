#include <stdio.h>
#include <vector>
//#include <functional> // bind2nd
//#include <algorithm> // countif, less

bool less2(int i)
{
    return i < 4;
}//less2()

struct less3
{
    bool operator()(int left)
    {
        return left < 4;
    }
};

template<typename T, typename U>
int count_if(T first, T last, U predicate_)
{
    int ret = 0;
    while (first != last)
    {
        if (predicate_(*first))
        {
            ret++;
        }
        ++first;
    }
    return ret;
}//count_if()

void main()
{
    int v[]{ 5,4,3,2,1 }; // initializer-list
    int count;

    count = count_if(&v[0], &v[_countof(v)], less2);
    printf("%i\r\n", count);
    count = count_if(&v[0], &v[_countof(v)], less3());
    printf("%i\r\n", count);
}//main()
