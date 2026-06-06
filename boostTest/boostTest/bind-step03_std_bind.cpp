#include <stdio.h>
#include <vector>
#include <functional> // bind2nd
#include <algorithm> // countif, less

bool less2( int i )
{
    return i < 4;
}//less2()

//template<typename T, typename U>
//int count_if( T first, T last, U predicate_ )
//{
//    int ret = 0;
//    while( first != last )
//    {
//        if( predicate_( *first ) )
//        {
//            ret++;
//        }
//        ++first;
//    }
//    return ret;
//}//count_if()
//
//template<typename T>
//class less
//{
//public:
//    typedef const T         argument_type;
//    typedef bool            result_type;
//    result_type operator()( argument_type& left_, argument_type& right_ ) const
//    {
//        return left_ < right_;
//    }
//};
//
//template<typename P>
//class binder2nd
//{
//public:
//    typedef typename P::argument_type   argument_type;
//    typedef typename P::result_type     result_type;
//
//    binder2nd( const P& pred_, argument_type& right_ ) : pred(pred_), right( right_ )
//    {
//    }
//    result_type operator()( argument_type& left_ ) const
//    {
//        return pred( left_, right );
//    }
//
//    P               pred;
//    argument_type   right;
//};//class binder2nd
//
//template<typename P>
//binder2nd<P> bind2nd( const P& pred_, typename P::argument_type& right_ )
//{
//    return binder2nd<P>( pred_, right_ );
//}//bind2nd()

using namespace std::placeholders;// ::_1;

void main()
{
    int v[] {5,4,3,2,1}; // initializer-list
    int count;

    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ], less2 );
    printf( "%i\r\n", count );
    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ], std::binder2nd<std::less<int>>( std::less<int>(), 4 ) );
    printf( "%i\r\n", count );
    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ], std::bind2nd( std::less<int>(), 4 ) );
    printf( "%i\r\n", count );
    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ], std::bind( std::less<int>(), _1, 4 ) );
    printf( "%i\r\n", count );
    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ]
        , std::bind( std::logical_and<bool>(),
        std::bind( std::less<int>(), _1, 4 ),
        std::bind( std::greater<int>(), _1, 1 ) ) );
    printf( "%i\r\n", count );
    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ], []( int i ) { return i < 4 && i > 1; } );
    printf( "%i\r\n", count );

    //std::vector<int>    v2{ 6, 5, 4, 3, 2, 1 };
    //count = count_if( v2.begin(), v2.end(), less2 );
    //printf( "%i\r\n", count );
    //count = count_if( v2.begin(), v2.end(), binder2nd<less<int>>( 4 ) );
    //printf( "%i\r\n", count );
    //count = count_if( v2.begin(), v2.end(), bind2nd( less<int>(), 4 ) );
    //printf( "%i\r\n", count );
    //count = count_if( v2.begin(), v2.end(), []( int i ) { return i < 4; } );
    //printf( "%i\r\n", count );
}//main()
