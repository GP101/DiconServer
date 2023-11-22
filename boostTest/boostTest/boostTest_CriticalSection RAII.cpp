#include <iostream>
#include <typeinfo>
#include <windows.h>
#include "KGen.h"

struct KCriticalSection : public CRITICAL_SECTION
{
    KCriticalSection( ) { InitializeCriticalSection( this ); }
    ~KCriticalSection( ) { DeleteCriticalSection( this ); }
};

class KCriticalSectionLock
{
public:
                        KCriticalSectionLock( CRITICAL_SECTION& cs ) : m_pcs( &cs )
                        {
                            printf( "constructor\r\n" );
                            assert( m_pcs != nullptr );
                            EnterCriticalSection( m_pcs );
                        }

                        ~KCriticalSectionLock( )
                        {
                            printf( "destructor\r\n" );
                            if( m_pcs != nullptr )
                                LeaveCriticalSection( m_pcs );
                        }
    explicit            operator bool() { return true; }
protected:
    CRITICAL_SECTION*   m_pcs;
};

void main( )
{
    printf( "hello\r\n" );
    KCriticalSection cs;
    if( KCriticalSectionLock lock = cs )
    {
        printf( "inside if\r\n" );
    }
    printf( "world\r\n" );
}