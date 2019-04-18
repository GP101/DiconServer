#include "NetError.h"
#include <map>
#define _WINSOCKAPI_
#include <Windows.h>
#include "KCriticalSection.h"


namespace NetError
{
#undef _ENUM
#define _ENUM( id, comment ) L#comment,
    wchar_t* szErrorStr[] = {
        #include "NetError.inc"
    };

    // key : thread id. element : last error
    std::map<DWORD, int> mapLastError;
    KCriticalSection  csNetError;
}

wchar_t* NetError::GetErrorStr( int nErrorId )
{
    KCriticalSectionLock lock( csNetError );

    if( nErrorId >= ERR_SENTINEL || nErrorId < 0 )
        return szErrorStr[ ERR_SENTINEL ];

    return szErrorStr[ nErrorId ];
}

void NetError::SetLastError( int nError_ )
{
    KCriticalSectionLock lock( csNetError );

    DWORD dwThreadId = ::GetCurrentThreadId();

    std::map<DWORD, int>::iterator mit;
    mit = mapLastError.find( dwThreadId );
    if( mit == mapLastError.end() ) {
        mapLastError.insert( std::make_pair( dwThreadId, nError_ ) );
        return;
    }

    mit->second = nError_;
}

int NetError::GetLastError()
{
    KCriticalSectionLock lock( csNetError );

    std::map<DWORD, int>::iterator mit;
    mit = mapLastError.find( ::GetCurrentThreadId() );

    if( mit == mapLastError.end() )
        return 0;

    return mit->second;
}

wchar_t* NetError::GetLastErrorMsg()
{
    KCriticalSectionLock lock( csNetError );

    return GetErrorStr( GetLastError() ); 
}
