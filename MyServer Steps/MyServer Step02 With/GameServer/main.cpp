#include <vector>
#include <memory>
#include "KPacket.h"
#include "KBaseServer.h"
#include "KMyGameServer.h"
#include "conio.h"

#define ESC_KEY         27

void StartupServer()
{
    KMyGameServer::CreateInstance<KMyGameServer>( 0L );
    assert( _KMyGameServer != nullptr );
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
    while( true ) {

        _KMyGameServer->Update( 0L );
        if( _kbhit() ) {
            const int ch = _getch();
            if( ch == ESC_KEY ) {
                break;
            }
        }
        Sleep( 10 );
    }

    ShutdownServer();
}
