#include <vector>
#include <boost/shared_ptr.hpp>
#include "KPacket.h"
#include "KBaseServer.h"
#include "KMyGameServer.h"
#include "conio.h"


void StartupServer()
{
    KMyGameServer::CreateInstance<KMyGameServer>( 0L );
    BOOST_ASSERT( _KMyGameServer != nullptr );
    _KMyGameServer->SetHwnd( NULL );

    const bool isInitialized = _KMyGameServer->Initialize( 0L );
    if( isInitialized == true ) {
        //_KMyGameServer->Main();
    }
}

void ShutdownServer()
{
    if( _KMyGameServer == NULL )
        return;

    _KMyGameServer->Finalize();
    std::cout.flush();
    std::wcout.flush();
}

void main()
{
    StartupServer();

    printf( "press any key to exit...\r\n" );
    _getch();

    ShutdownServer();
}
