#pragma once

typedef int NetErr;

namespace NetError
{
    enum NETWORK_ERROR 
    {
        NET_OK = 0, // success
        ERR_UNKNOWN, // unknown error

        ERR_VERIFY_00, // duplicate login for a server
        ERR_VERIFY_01, // duplicate login for different servers
        ERR_VERIFY_02, // failed of Ip verification
        ERR_VERIFY_03, // duplicate Uid in server
        ERR_CHAR_00, // failed in character creation

        ERR_SENTINEL,
    };

    void                SetLastError( int nError );
    int                 GetLastError();
}
