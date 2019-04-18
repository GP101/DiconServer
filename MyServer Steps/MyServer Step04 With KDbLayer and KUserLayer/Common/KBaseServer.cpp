#include <WinSock2.h>
#include "KBaseServer.h"
#include "KGen.h"
#include "NetError.h"
#include "KThreadManager.h"
#include "NetUtil.h"
#include "KNetLayer.h"
#include "KUserLayer.h"
#include "KDbLayer.h"
#include <boost/bind.hpp>

#pragma comment( lib, "ws2_32" )

extern const wchar_t* g_szConfigFile;


CONSTRUCTOR KBaseServer::KBaseServer() 
    : m_strTagVersion( L"(undefined tag version" )
    , m_nProtocolVersion( 0 )
    , m_dwShutDownWaitTime( 1000 * 60 * 2 )
    , m_hWnd( HWND_DESKTOP )
{
    srand( (unsigned) time( NULL ) );
    m_dwServerInfoUpdateInterval = ::GetTickCount();
}

DESTRUCTOR KBaseServer::~KBaseServer()
{
}

bool KBaseServer::Initialize( DWORD dwParam_ )
{
    KDbLayer::CreateInstance<KDbLayer>( 0 ); // _KDbLayer
    KNetLayer::CreateInstance<KNetLayer>( 0 ); // _KNetLayer

    const bool isInitializeWinsock = NetUtil::InitializeWinSock();
    if( isInitializeWinsock == false ) {
        BEGIN_LOG( cerr, L"error" )
            << END_LOG;
        return false;
    }

    LoadIniFile( g_szConfigFile );

    _KNetLayer->Initialize( reinterpret_cast<DWORD>( &m_netLayerInitParam ) );
    _KUserLayer->Initialize( reinterpret_cast<DWORD>( &m_userLayerInitParam) ); // initialize and begin threads
    _KDbLayer->Initialize( reinterpret_cast<DWORD>( &m_dbLayerInitParam ) ); // initialize and begin threads

    // initialize Db
    KNetAddress kPacket;
    {
        kPacket.m_wstrIp = NetUtil::GetLocalIpW();
        kPacket.m_usPort = _KNetLayer->GetPort();
    }

    NetSendTo( SNDTO_DB, 0, EDB_ON_START_REQ, kPacket );

    return true;
}

VIRTUAL void KBaseServer::Finalize()
{
    _KDbLayer->Finalize(); // end threads and finalize

    // session finalization will be done in derived class.
    const bool isDestroyAllSession = DestroyAllSession();
    if( isDestroyAllSession == false ) {
        BEGIN_LOG( cout, L"error" );
    }

    _KUserLayer->Finalize(); // end threads and finalize
    _KNetLayer->Finalize();

    NetUtil::FinalizeWinsock();
}

VIRTUAL void KBaseServer::Update(DWORD dwElapsedTime_)
{
    m_workQueue.ProcessPackets( boost::bind( &KBaseServer::OnPacket, this, _1 ) );
}

void KBaseServer::QueueingPacket( const KPacketPtr& spPacket )
{
    m_workQueue.QueueingPacket( spPacket );
}

namespace ConfigINI 
{
    /*//// ...in ini file... ////////////////////////////////////////////////////

    [Version]
    Protocol Version 1 = XXX
    Protocol Version 2 = YYY
    
    [String]
    aaa = AAA
    bbb = BBB
    ...

    /////////////////////////////////////////////////////////////////////////*/

    const wchar_t* szSectionVersion = L"Version";
    const wchar_t* szSectionString  = L"String";
    const wchar_t* szKeyVersion1    = L"Protocol Version 1";
    const wchar_t* szKeyVersion2    = L"Protocol Version 2";
}

void KBaseServer::LoadIniFile( const wchar_t* szFileName_ )
{
    // use absolute path or relative path which start with ./ or ../
    wchar_t buff[MAX_PATH] = {0};
    ::GetCurrentDirectoryW( MAX_PATH, buff );
    m_strIniFilePath = buff;
    m_strIniFilePath += L"\\";
    m_strIniFilePath += szFileName_;
    m_strIniFilePath += L".ini";

    // get protocol version
    m_nProtocolVersion  = ::GetPrivateProfileIntW( ConfigINI::szSectionVersion, ConfigINI::szKeyVersion1, 0, m_strIniFilePath.c_str() );

    if( m_nProtocolVersion == 0 ) {
        BEGIN_LOG( cerr, L"Cannot cat version information" )
            << LOG_NAMEVALUE( m_strIniFilePath )
            << LOG_NAMEVALUE( m_nProtocolVersion );
    }
}

void KBaseServer::SetProtocolVersion( int nProtocolVer_ )
{
    BEGIN_LOG( cout, L"change m_nProtocolVersion (" << m_nProtocolVersion << L" -> " << nProtocolVer_ << L")" );

    m_nProtocolVersion = nProtocolVer_;

    wchar_t buff[32] = {0};
    ::_itow_s( m_nProtocolVersion, buff, 10 );
    if( !::WritePrivateProfileStringW( ConfigINI::szSectionVersion, ConfigINI::szKeyVersion1
        , buff, m_strIniFilePath.c_str() ) )
    {
        BEGIN_LOG( cout, L"error" );
    }
}

void KBaseServer::SetServerList( IN const std::vector< KServerInfo >& vecServerList_ )
{
    CSLOCK( m_csServerInfo )
    {
        m_vecServerInfoList = vecServerList_;
    }
}

void KBaseServer::GetServerList( OUT std::vector< KServerInfo >& vecServerList_ )
{
    CSLOCK( m_csServerInfo )
    {
        vecServerList_ = m_vecServerInfoList;
    }
}