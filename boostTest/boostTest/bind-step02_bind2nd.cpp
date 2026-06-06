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
    while (first != last) {
        if (predicate_(*first)) {
            ret++;
        }
        ++first;
    }
    return ret;
}//count_if()

template<typename T>
class less
{
public:
    typedef const T         argument_type;
    typedef bool            result_type;
    result_type operator()(argument_type& left_, argument_type& right_) const
    {
        return left_ < right_;
    }
};

template<typename P>
class binder2nd
{
public:
    typedef typename P::argument_type   argument_type;
    typedef typename P::result_type     result_type;

    binder2nd(const P& pred_, argument_type& right_)
        : pred(pred_), right(right_)
    {}
    result_type operator()(argument_type& left_) const
    {
        return pred(left_, right);
    }

private:
    P               pred;
    argument_type   right;
};//class binder2nd

//template<typename P>
//binder2nd<P> bind2nd(const P& pred_, typename P::argument_type& right_)
//{
//    return binder2nd<P>(pred_, right_);
//}//bind2nd()

void main()
{
    int v[]{ 5,4,3,2,1 }; // initializer-list
    int count;

    count = count_if(&v[0], &v[_countof(v)], less2);
    printf("%i\r\n", count);
    count = count_if(&v[0], &v[_countof(v)], less3());
    printf("%i\r\n", count);
    count = count_if(&v[0], &v[_countof(v)], binder2nd<less<int>>(less<int>(), 4));
    printf("%i\r\n", count);
    //count = count_if(&v[0], &v[_countof(v)], bind2nd(less<int>(), 4));
    //printf("%i\r\n", count);
}//main()
