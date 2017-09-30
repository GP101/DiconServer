#include <stdio.h>
#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <conio.h>

#define MAX_THREADS     2

DWORD   g_dwThreadId;
HANDLE  g_hThread;
DWORD   g_dwData = 9;

DWORD WINAPI ThreadCallback( LPVOID lpParam );

void main()
{
    g_hThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        ThreadCallback,       // thread function name
        (LPVOID) &g_dwData,          // argument to thread function 
        0,                      // use default creation flags 
        &g_dwThreadId );   // returns the thread identifier 

    std::cout << "hello" << std::endl;

    _getch();

    std::cout << "world" << std::endl;

    CloseHandle( g_hThread );
}

DWORD WINAPI ThreadCallback( LPVOID lpParam )
{
    DWORD dwData = *((DWORD*) lpParam);
    std::cout << "Parameter = " << dwData << std::endl;
    Sleep( 1000 );

    return 0;
}
