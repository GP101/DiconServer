//#include "KSession.h"
#include "KIocpWorkerThread.h"
#include "KGen.h"
#include "KIocp.h"


CONSTRUCTOR KIocpWorkerThread::KIocpWorkerThread()
{
}

DESTRUCTOR KIocpWorkerThread::~KIocpWorkerThread()
{
}

VIRTUAL void KIocpWorkerThread::ThreadRun()
{
    BEGIN_LOG( cout, L"KIocpWorkerThread Enter ThreadRun" )
        << END_LOG;

    DWORD dwPrevTime = ::GetTickCount();

    while( true ) {
        const DWORD dwCurrTime = ::GetTickCount();
        const DWORD dwElapsedTime = dwCurrTime - dwPrevTime;
        dwPrevTime = dwCurrTime;

        const DWORD ret = ::WaitForSingleObject( m_hQuitEvent, 0 );
        if( ret == WAIT_OBJECT_0 )
            break;
        else if( ret == WAIT_TIMEOUT )
            ThreadUpdateLoop( dwElapsedTime );
        else {
            BEGIN_LOG( cerr, L"*** WaitForSingleObject() - return : " )
                << LOG_NAMEVALUE( ret )
                << END_LOG;
        }
    }// while

    BEGIN_LOG( cout, L"KIocpWorkerThread Exit ThreadRun" )
        << END_LOG;
}

VIRTUAL void KIocpWorkerThread::ThreadUpdateLoop( DWORD dwElapsedTime_ )
{
    LPOVERLAPPED povl = NULL;
    DWORD dwBytesTransfer = 0;
    DWORD dwKey = 0;

    const bool bOK = _KIocp.GetCompletionStatus( OUT &dwKey, OUT &dwBytesTransfer, OUT &povl );

    if( KIocp::QUIT_IOCP_WORKER_THREAD == dwKey && povl == NULL ) {
        BEGIN_LOG( cout, L"QUIT_IOCP_WORKER_THREAD entered." )
            << END_LOG;
        SetEvent( m_hQuitEvent ); // force quit ThredRun().
        return;
    } // if

    if( bOK == true ) {
        _KIocp.OnIoCompleted( dwKey, dwBytesTransfer, povl );
    }
    else {
        BEGIN_LOG( cout, L"closed by local machine. dwKey : " << dwKey );
        _KIocp.OnClosedByLocal( dwKey );
    }
}
