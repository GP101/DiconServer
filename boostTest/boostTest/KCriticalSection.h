#pragma once
#include <boost/assert.hpp>
#include "KGen.h"

struct KCriticalSection : public CRITICAL_SECTION
{
    CONSTRUCTOR         KCriticalSection( ) { InitializeCriticalSection( this ); }
    DESTRUCTOR          ~KCriticalSection( ) { DeleteCriticalSection( this ); }
};

class KCriticalSectionLock
{
public:
    CONSTRUCTOR         KCriticalSectionLock( CRITICAL_SECTION& cs ) : m_pcs( &cs )
                        {
                            //printf( "constructor\r\n" );
                            BOOST_ASSERT( m_pcs != nullptr );
                            EnterCriticalSection( m_pcs );
                        }

    DESTRUCTOR          ~KCriticalSectionLock( )
                        {
                            //printf( "destructor\r\n" );
                            if( m_pcs != nullptr )
                                LeaveCriticalSection( m_pcs );
                        }
    explicit            operator bool( ) { return true; }
protected:
    CRITICAL_SECTION*   m_pcs;
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
