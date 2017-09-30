#include <stdio.h>
#include <windows.h>
#include <strsafe.h>
#include <iostream>

#define MAX_THREADS     2

DWORD   g_dwThreadIdArray[MAX_THREADS];
HANDLE  g_hThreadArray[MAX_THREADS];
DWORD   g_dwDataArray[MAX_THREADS] = {1,3};
CRITICAL_SECTION    g_csConsole;
CRITICAL_SECTION    g_csShared;
int     g_iShared = 0;

DWORD WINAPI ThreadCallback( LPVOID lpParam );

void main()
{
    InitializeCriticalSection( &g_csConsole );
    InitializeCriticalSection( &g_csShared );

    std::cout << "shared = " << g_iShared << std::endl;

    for( int i = 0; i < MAX_THREADS; ++i ) {
        g_hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ThreadCallback,       // thread function name
            (LPVOID) &g_dwDataArray[i],          // argument to thread function 
            0,                      // use default creation flags 
            &g_dwThreadIdArray[i] );   // returns the thread identifier 
    }

    std::cout << "hello" << std::endl;

    WaitForMultipleObjects( MAX_THREADS, g_hThreadArray, TRUE, INFINITE );

    std::cout << "world" << std::endl;
    std::cout << "shared = " << g_iShared << std::endl;

    for( int i = 0; i<MAX_THREADS; i++ ) {
        CloseHandle( g_hThreadArray[i] );
    }

    DeleteCriticalSection( &g_csConsole );
    DeleteCriticalSection( &g_csShared );
}

DWORD WINAPI ThreadCallback( LPVOID lpParam )
{
    DWORD dwData = *((DWORD*)lpParam);

    const int iCounter = 100000;
    if( dwData == 1 ) {
        for( int i = 0; i < iCounter; ++i ) {
            //EnterCriticalSection( &g_csShared );
            g_iShared += 1;
            //LeaveCriticalSection( &g_csShared );
        }
    }
    else {
        for( int i = 0; i < iCounter; ++i ) {
            //EnterCriticalSection( &g_csShared );
            g_iShared -= 1;
            //LeaveCriticalSection( &g_csShared );
        }
    }
    EnterCriticalSection( &g_csConsole );
    std::cout << "Parameter = " << dwData << std::endl;
    LeaveCriticalSection( &g_csConsole );
    Sleep( 1000 );

    return 0;
}
