#include <iostream>
#include <typeinfo>

int main( )
{
    int * a;
    int b;

    a = 0; b = 0;
    if( typeid( a ) != typeid( b ) )
    {
        std::cout << "a and b are of different types:\n";
        std::cout << "a is: " << typeid( a ).name( ) << '\n';
        std::cout << "b is: " << typeid( b ).name( ) << '\n';
    }
    return 0;
}