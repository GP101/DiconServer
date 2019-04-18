#include "NetError.h"
#include <map>
#define _WINSOCKAPI_
#include <Windows.h>
#include "KCriticalSection.h"


namespace NetError
{
    // key : thread id. element : last error
    std::map<DWORD, int> mapLastError;
    KCriticalSection  csNetError;
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
