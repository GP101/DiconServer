#pragma once

typedef int NetErr;

namespace NetError
{

#undef _ENUM
#define _ENUM( id, comment ) id,
    enum NETWORK_ERROR 
    {
        #include "NetError.inc"
    };

    wchar_t*            GetErrorStr( int nErrorId_ );

    void                SetLastError( int nError );
    wchar_t*            GetLastErrorMsg();
    int                 GetLastError();
}
