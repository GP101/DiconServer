#include <stdio.h>
#include <vector>
#include <functional> // bind2nd
#include <algorithm> // count_if, less

using namespace std::placeholders;// ::_1;
//using std::placeholders::_2;

void main()
{
    int v[]{ 5, 4, 3, 2, 1 }; // initializer-list
    int count = 0;

    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ], std::bind( std::less<int>(), _1, 4 ) );
    printf( "%i\r\n", count );

    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ], []( int left ) { return left < 4; } );
    printf( "%i\r\n", count );

    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ],
        std::bind( std::logical_and<bool>(),
            std::bind( std::less<int>(), _1, 4 ),
            std::bind( std::greater<int>(), _1, 1 ) ) );
    printf( "%i\r\n", count );

    count = std::count_if( &v[ 0 ], &v[ _countof( v ) ]
        , []( int left ) { return left < 4 && left > 1; } );
    printf( "%i\r\n", count );
}//main()
