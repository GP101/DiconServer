#include <WinSock2.h>
#include "KBaseServer.h"
#include "KGen.h"
#include "NetError.h"
#include "KThreadManager.h"
#include "NetUtil.h"
#include "KNetLayer.h"
#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

#pragma comment( lib, "ws2_32" )

extern const wchar_t* g_szConfigFile;


CONSTRUCTOR KBaseServer::KBaseServer() 
    : m_hWnd( HWND_DESKTOP )
{
    srand( (unsigned) time( NULL ) );
}

DESTRUCTOR KBaseServer::~KBaseServer()
{
}

bool KBaseServer::Initialize( DWORD dwParam_ )
{
    KNetLayer::CreateInstance<KNetLayer>( 0 ); // _KNetLayer

    const bool isInitializeWinsock = NetUtil::InitializeWinSock();
    if( isInitializeWinsock == false ) {
        BEGIN_LOG( cerr, L"error" )
            << END_LOG;
        return false;
    }

    LoadIniFile( g_szConfigFile );

    _KNetLayer->Initialize( reinterpret_cast<DWORD>( &m_netLayerInitParam ) );

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
    // session finalization will be done in derived class.
    const bool isDestroyAllSession = DestroyAllSession();
    if( isDestroyAllSession == false ) {
        BEGIN_LOG( cout, L"error" );
    }

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
}
