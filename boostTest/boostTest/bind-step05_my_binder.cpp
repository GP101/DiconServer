#include <stdio.h>
#include <functional>
#include <algorithm>

template<typename T>
class binder
{
    typedef T type;
};

template<typename T>
class binder<T( *)>
{
public:
    void operator()( T pred, int value )
    {
        pred( value );
    }
};

template<typename T, typename U>
class binder<T( U::* )>
{
public:
    using PRED = int ( U::* )( int );

    binder( PRED pred = nullptr ) : mpred( pred ) {}

    int operator()( U& left, int value )
    {
        return ( left.*mpred )( value );
    }

    int operator()( PRED pred, U& left, int value )
    {
        return ( left.*pred )( value );
    }

    int operator()( int ( U::*pred )( int, int ), U& left, int first, int second )
    {
        return ( left.*pred )( first, second );
    }

private:
    PRED mpred;
};

template<typename T>
typename binder<T> bind( T pred )
{
    return binder<T>( pred );
}

int Print( int iData )
{
    printf( "%i\r\n", iData );
    return 0;
}

class KTest
{
public:
    int Print( int iData )
    {
        printf( "%i\r\n", iData );
        return iData;
    }
    int Print2( int first, int second )
    {
        printf( "%i, %i\r\n", first, second );
        return first + second;
    }
};//class KTest

using namespace std::placeholders;// ::_1;
//using std::placeholders::_2;

void main()
{
    KTest   t;

    std::bind( &KTest::Print, t, 1 )( );
    std::bind( &KTest::Print, t, _1 )( 2 );
    std::bind( &KTest::Print, _1, 3 )( t );
    std::bind( &KTest::Print, _1, _2 )( t, 4 );
    std::bind( &KTest::Print, _2, _1 )( 5, t );

    //typedef int (KTest::*KTEST_PRINT)(int);
    using KTEST_PRINT = int ( KTest::* )( int ); // type aliasing
    KTEST_PRINT pPrint = &KTest::Print;
    ( t.*pPrint )( 6 );

    binder<int( *)( int )>    c;
    c( &Print, 7 );

    binder<int ( KTest::* )( int )>  d;
    d( &KTest::Print, t, 8 );
    d( &KTest::Print2, t, 9, 10 );

    bind( &KTest::Print )( t, 11 );
}//main()
