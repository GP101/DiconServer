#include <WinSock2.h>
#include <sstream>
#include <iomanip>
#include "KMyGameServer.h"
#include "KUser.h"
#include "KMyUserFsm.h"
#include "KLog.h"

using namespace std::placeholders;

const wchar_t*  g_szConfigFile = L"config";
//{{ qff. jintaeks on 2017-09-21_18-57
KUserPtr    g_spClientUser;
//}} qff


KUserPtr CreateUser()
{
    KUserPtr spUser;
    spUser.reset( new KUser() );
    g_spClientUser = spUser;
    g_spClientUser->Initialize( 0L );
    BEGIN_LOG(clog, L"") << END_LOG;
    return spUser;
}

KFsmPtr CreateUserFsm()
{
    KFsmPtr spFsm;
    spFsm.reset( new KMyUserFsm() );
    return spFsm;
}


CONSTRUCTOR KMyGameServer::KMyGameServer()
{
}

DESTRUCTOR KMyGameServer::~KMyGameServer()
{
}

VIRTUAL bool KMyGameServer::Initialize( DWORD dwParam_ )
{
    m_netLayerInitParam.dwNumIocpThreads = 4;
    m_netLayerInitParam.pfnCreateUserCallback = &CreateUser;

    const bool isInitialized = KBaseServer::Initialize( dwParam_ );
    if( isInitialized == false ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
        return false;
    }

    BEGIN_LOG( clog, L"" ) << END_LOG;

    return true;
}

VIRTUAL void KMyGameServer::Finalize()
{
    if( g_spClientUser != nullptr )
        g_spClientUser->Finalize();
    __super::Finalize();
}

bool KMyGameServer::DestroyAllSession()
{
    //// at this point all Iocp accepters of NetInterface are closed.

    return true;
}

VIRTUAL void KMyGameServer::Update(DWORD dwElapsedTime_)
{
    VIRTUAL KBaseServer::Update(dwElapsedTime_);
}

VIRTUAL void KMyGameServer::OnPacket( IN KPacket* pkPacket )
{
    KLOG << "Received Packet " << pkPacket->m_usPacketId;
}
