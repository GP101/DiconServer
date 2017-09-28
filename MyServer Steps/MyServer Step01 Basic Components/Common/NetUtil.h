#pragma once
#define _WINSOCKAPI_
#include <Windows.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <atltime.h>
#include <boost/format.hpp>
#include <sstream>


namespace NetUtil
{
    bool                InitializeWinSock();
    void                FinalizeWinsock();

    LONGLONG            GetTempUid();
    const wchar_t*      GetWsaMsg();
    const char*         GetLocalIp();
    std::wstring        GetLocalIpW();
    const char*         CalcIp( DWORD dwIP );
    void                GetAppName( OUT std::wstring& strAppName );
    time_t              TimeToInt( CTime& cTime_ );
    std::string         WideToNarrowString( const wchar_t* pStr, int len = -1 );
    std::string         WideToNarrowString( const std::wstring& str );
    std::wstring        NarrowToWideString( const char* pStr, int len = -1 );
    std::wstring        NarrowToWideString( const std::string& str );
}
