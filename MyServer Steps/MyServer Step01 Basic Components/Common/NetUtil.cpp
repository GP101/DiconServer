#include <ws2tcpip.h>
#include "NetUtil.h"
#include <string>
#include <boost/random.hpp>
#include "KCriticalSection.h"
#include <DbgHelp.h>

#pragma comment( lib, "DbgHelp" )


namespace NetUtil
{
    wchar_t WSAMsg[256] = { 0 };

    boost::mt19937 rng;
    boost::uniform_int<DWORD> uint32( 1, UINT_MAX );
    boost::uniform_int<LONGLONG> uint40( 1, 0x000000ffffffffff );
    boost::variate_generator<boost::mt19937&, boost::uniform_int<DWORD> > die32( rng, uint32 );
    boost::variate_generator<boost::mt19937&, boost::uniform_int<LONGLONG> > die40( rng, uint40 );

    KCriticalSection m_cs;
}

bool NetUtil::InitializeWinSock()
{
    WORD wVer = MAKEWORD( 2, 2 );
    WSADATA wsaData;

    if( ::WSAStartup( wVer, OUT &wsaData ) != 0 ) {
        ::WSACleanup();
        return false;
    }

    if( wsaData.wVersion != MAKEWORD( 2, 2 ) ) {
        ::WSACleanup();
        return false;
    }

    return true;
}

void NetUtil::FinalizeWinsock()
{
    ::WSACleanup();
}

const wchar_t* NetUtil::GetWsaMsg()
{
    FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM, NULL, ::WSAGetLastError()
        , GetSystemDefaultLangID()/*MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/
        , WSAMsg, MAX_PATH, NULL );
    WSAMsg[wcslen( WSAMsg ) - 1] = L'\0'; // remove new line character by adding EOS mark.

    return WSAMsg;
}

const char* NetUtil::GetLocalIp()
{
    char szHostName[256];
    PHOSTENT pHostInfo;

    if( gethostname( szHostName, sizeof( szHostName ) ) != 0 ) {
        return false;
    }

    pHostInfo = gethostbyname( szHostName );

    if( pHostInfo == NULL ) {
        return false;
    }

    char buffer[80];

    return inet_ntop( AF_INET, (void*) ( ( struct in_addr * )*pHostInfo->h_addr_list ), buffer, sizeof( buffer ) );
    //return inet_ntoa(*(struct in_addr *)*pHostInfo->h_addr_list); 
}

std::wstring NetUtil::GetLocalIpW()
{
    std::string strLocalIP = GetLocalIp();
    std::wstring wstrLocalIP = NetUtil::NarrowToWideString( strLocalIP.c_str(), strLocalIP.length() );

    return wstrLocalIP;
}

const char* NetUtil::CalcIp( DWORD dwIP )
{
    in_addr IP;
    IP.s_addr = dwIP;
    //return inet_ntoa( IP );
    char buffer[80];
    return inet_ntop(AF_INET, (void*)(&IP), buffer, sizeof(buffer));
}

LONGLONG NetUtil::GetTempUid()
{
    KCriticalSectionLock lock( m_cs );
    return die40() | (LONGLONG) 0x1 << 62;
}

// get executable filename
void NetUtil::GetAppName( OUT std::wstring& strAppName )
{
    wchar_t szFileName[_MAX_PATH];
    ::GetModuleFileNameW( NULL, szFileName, _MAX_FNAME );

    CStringW sAppName; // Extract from last '\' to '.'
    sAppName = szFileName;
    strAppName = sAppName.Mid( sAppName.ReverseFind( L'\\' ) + 1 ).SpanExcluding( L"." );
}

time_t NetUtil::TimeToInt( CTime& cTime_ )
{
    struct tm tmBuff;
    cTime_.GetLocalTm( &tmBuff );
    return ::mktime( &tmBuff );
}

std::string NetUtil::WideToNarrowString( const wchar_t* pStr, int len /*= -1*/ )
{
    std::string buf;
    if( pStr == NULL )
        return buf;
    if( len != -1 && len <= 0 ) // check empty string
        return buf;

    // figure out how many narrow characters we are going to get 
    int nChars = WideCharToMultiByte( CP_ACP, 0, pStr, len, NULL, 0, NULL, NULL );
    if( len == -1 )
        --nChars;
    if( nChars == 0 )
        return "";

    // convert the wide string to a narrow string
    // nb: slightly naughty to write directly into the string like this
    buf.resize( nChars );
    WideCharToMultiByte( CP_ACP, 0, pStr, len
        , const_cast<char*>( buf.c_str() ), nChars, NULL, NULL );

    return buf;
}

std::string NetUtil::WideToNarrowString( const std::wstring& str )
{
    return WideToNarrowString( str.c_str() );
}

std::wstring NetUtil::NarrowToWideString( const char* pStr, int len /*= -1*/ )
{
    std::wstring buf;
    if( pStr == NULL )
        return buf;
    if( len != -1 && len <= 0 )
        return buf;

    // figure out how many wide characters we are going to get 
    int nChars = MultiByteToWideChar( CP_ACP, 0, pStr, len, NULL, 0 );
    if( len == -1 )
        --nChars;
    if( nChars == 0 )
        return L"";

    // convert the narrow string to a wide string 
    // nb: slightly naughty to write directly into the string like this
    buf.resize( nChars );
    MultiByteToWideChar( CP_ACP, 0, pStr
        , len, const_cast<wchar_t*>( buf.c_str() ), nChars );

    return buf;
}

std::wstring NetUtil::NarrowToWideString( const std::string& str )
{
    return NarrowToWideString( str.c_str() );
}
