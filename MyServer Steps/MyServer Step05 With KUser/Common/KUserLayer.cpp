#include <Winsock2.h>   
#include "KUserLayer.h"
#include "KNetLayer.h"
#include "KBaseServer.h"
#include "KUserManager.h"
#include "KCriticalSection.h"
#include <sstream>
#include <functional>

using namespace std::placeholders;

CONSTRUCTOR KUserLayer::KUserLayer()
{
}

DESTRUCTOR KUserLayer::~KUserLayer()
{
    BEGIN_LOG( cout, L"~KUserLayer" );
    assert( GetThreadId() == 0 );
}

bool KUserLayer::Initialize( DWORD dwParam_ )
{
    KUserLayerInitParam* pInitParam = reinterpret_cast<KUserLayerInitParam*>( dwParam_ );
    assert( pInitParam != nullptr );

    VIRTUAL _KUserManager->Initialize( dwParam_ );

    VIRTUAL BeginThread();

    return true;
}

VIRTUAL void KUserLayer::Finalize()
{
    if( m_hThreadEvent != NULL )
        VIRTUAL EndThread( 3000 );

    VIRTUAL _KUserManager->Finalize();
}

void KUserLayer::ThreadRun()
{
    BEGIN_LOG( cout, L"Enter" )
        << END_LOG;

    DWORD dwPrevTime = ::GetTickCount( );

    while( true ) {
        const DWORD dwCurrTime = ::GetTickCount();
        const DWORD dwElapsedTime = dwCurrTime - dwPrevTime;
        dwPrevTime = dwCurrTime;

        const DWORD ret = ::WaitForSingleObject( m_hQuitEvent, 1 );

        if( ret == WAIT_OBJECT_0 )
            break;

        VIRTUAL Update( dwElapsedTime );
    }

    BEGIN_LOG( cout, L"Exit" )
        << END_LOG;
}

VIRTUAL void KUserLayer::Update( DWORD dwElapsedTime_ )
{
	_KUserManager->Update(dwElapsedTime_);
	_KBaseServer->Update(dwElapsedTime_);
}
