#include <Winsock2.h>   
#include "KUserLayer.h"
#include "KNetLayer.h"
#include "KBaseServer.h"
#include "KCriticalSection.h"
#include <sstream>
#include <boost/bind.hpp>


CONSTRUCTOR KUserLayer::KUserLayer()
{
}

DESTRUCTOR KUserLayer::~KUserLayer()
{
    BEGIN_LOG( cout, L"~KUserLayer" );
    BOOST_ASSERT( GetThreadId() == 0 );
}

bool KUserLayer::Initialize( DWORD dwParam_ )
{
    KUserLayerInitParam* pInitParam = reinterpret_cast<KUserLayerInitParam*>( dwParam_ );
    BOOST_ASSERT( pInitParam != nullptr );

    VIRTUAL BeginThread();

    return true;
}

VIRTUAL void KUserLayer::Finalize()
{
    if( m_hThreadEvent != NULL )
        VIRTUAL EndThread( 3000 );
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
	_KBaseServer->Update(dwElapsedTime_);
}
