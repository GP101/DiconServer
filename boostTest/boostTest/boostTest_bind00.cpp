#include <boost/bind.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

void print(std::ostream *os, int i)
{
    *os << i << '\n';
}

void Print(int a, int b)
{
    printf("%i, %i\r\n", a, b);
}

int main()
{
    boost::bind(Print, _2, _1).operator()( 1, 33 );
    //std::vector<int> v{ 1, 3, 2 };
    return 0;
}