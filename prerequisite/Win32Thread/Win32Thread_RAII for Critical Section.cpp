#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>

struct KCriticalSection : public CRITICAL_SECTION
{
    KCriticalSection() { InitializeCriticalSection( this ); }
    ~KCriticalSection() { DeleteCriticalSection( this ); }
};

class KCriticalSectionLock
{
public:
    KCriticalSectionLock( CRITICAL_SECTION& cs ) : m_pcs( &cs )
    {
        EnterCriticalSection( m_pcs );
    }

    ~KCriticalSectionLock()
    {
        if( m_pcs != nullptr )
            LeaveCriticalSection( m_pcs );
    }
    explicit operator bool() { return true; }
protected:
    CRITICAL_SECTION*   m_pcs;
};

#define CSLOCK( cs_ )   if( KCriticalSectionLock lock = cs_ )


int                 g_value = 0;
KCriticalSection    g_csValue;

DWORD WINAPI ThreadProc(LPVOID);

int main(void)
{
    HANDLE hThread;
    DWORD dwThreadID;

    // Create a thread
    hThread = CreateThread(
        NULL,         // default security attributes
        0,            // default stack size
        (LPTHREAD_START_ROUTINE)ThreadProc,
        NULL,         // no thread function arguments
        0,            // default creation flags
        &dwThreadID); // receive thread identifier

    while( true )
    {
        for( int i = 0; i < 1000000; ++i )
        {
            CSLOCK( g_csValue )
            {
                g_value -= 1; // critical section
            }
        }

        break;
    }

    _getch();

    std::cout << g_value << std::endl;

    return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    while( true )
    {
        for( int i = 0; i < 1000000; ++i )
        {
            CSLOCK( g_csValue )
            {
                g_value += 1; // critical section
            }
        }

        break;
    }
    return 0;
}
