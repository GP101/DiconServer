#pragma once
#include <cassert>
#include "KGen.h"


struct KCriticalSection : public CRITICAL_SECTION
{
    CONSTRUCTOR         KCriticalSection() { InitializeCriticalSection( this ); }
    DESTRUCTOR          ~KCriticalSection() { DeleteCriticalSection( this ); }
};


class KCriticalSectionLock
{
public:
    CONSTRUCTOR         KCriticalSectionLock( const CRITICAL_SECTION& cs ) : m_pcs( &cs )
                        {
                            assert( m_pcs != nullptr );
                            EnterCriticalSection( (LPCRITICAL_SECTION)m_pcs );
                        }

    DESTRUCTOR          ~KCriticalSectionLock()
                        {
                            if( m_pcs != nullptr )
                                LeaveCriticalSection( (LPCRITICAL_SECTION)m_pcs );
                        }
    explicit            operator bool() { return true; }
protected:
    const CRITICAL_SECTION*
                        m_pcs;
};

#define CSLOCK( cs_ )   if( KCriticalSectionLock CONCATENATE( lock_, __LINE__ ) = cs_ )

/** @exam   KCriticalSection
#include <iostream>
#include "KGen.h"
#include "KCriticalSection.h"

void main( )
{
    printf( "hello\r\n" );
    KCriticalSection cs;
    CSLOCK( cs )
    {
        printf( "inside if\r\n" );
    }
    printf( "world\r\n" );
}

*/
