#pragma once
#include "KGen.h"
#include <string>
#include <map>
#include "KCriticalSection.h"
#include <process.h>

#pragma comment( lib, "Winmm" )


class KThreadManager;
class KThread
{
    typedef unsigned (__stdcall* PTHREAD_START)(void*);

public:
    CONSTRUCTOR         KThread() 
                            : m_hQuitEvent( NULL )
                            , m_hThreadEvent( NULL )
                            , m_pkThreadMgr( NULL )
                        {
                        }

    virtual             ~KThread()
                        {
                            //EndThread( 3000 );
                            BOOST_ASSERT( GetThreadId() == 0 );
                        }

    virtual bool        BeginThread()
                        {
                            if( m_hThreadEvent != NULL )
                                return false;

                            m_hQuitEvent = CreateEvent(NULL, false, false, NULL);
                            if( NULL == m_hQuitEvent )
                                return false;

                            m_hThreadEvent = (HANDLE)_beginthreadex( NULL, 0, (PTHREAD_START)ThreadProc
                                , (void*)this, 0, OUT (unsigned*)&m_dwThreadId );

                            if( NULL == m_hThreadEvent ) {
                                ::CloseHandle( m_hQuitEvent);
                                m_hQuitEvent = NULL;
                                return false;
                            } // if

                            return true;
                        }

    virtual void        EndThread( DWORD dwTimeout_ )
                        {
                            if( NULL != m_hThreadEvent ) {
                                SetEvent( m_hQuitEvent );
                                ::WaitForSingleObject( m_hThreadEvent, dwTimeout_ );
                                ::CloseHandle( m_hThreadEvent );
                                m_hThreadEvent = NULL;
                                m_dwThreadId = 0;
                            }
                            if( m_hQuitEvent != NULL ) {
                                ::CloseHandle( m_hQuitEvent );
                                m_hQuitEvent = NULL;
                            }
                        }
    
    virtual void        ThreadRun() = 0;
    
    DWORD               GetThreadId() const { return m_dwThreadId; }
    void                SetThreadManager( KThreadManager* pkMgr ) { m_pkThreadMgr = pkMgr; }

protected:
    static unsigned int WINAPI 
                        ThreadProc( LPVOID pvParam_ )
                        {
                            KThread* pThread = reinterpret_cast<KThread*>( pvParam_ );
                            BOOST_ASSERT( pThread != nullptr );
                            pThread->ThreadRun();
                            return true;
                        }
    
protected:
    HANDLE              m_hQuitEvent;
    HANDLE              m_hThreadEvent;
    DWORD               m_dwThreadId = 0;
    KThreadManager*     m_pkThreadMgr;
};
