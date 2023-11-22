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

        using namespace std::chrono_literals;
        std::unique_lock<std::mutex> lk(m_cvMutex);
        auto now = std::chrono::system_clock::now();
        if (m_cvQuit.wait_until(lk, now + 1ms, [this]() { return m_quitFlag == 1; }))
            break;

        VIRTUAL ThreadUpdateLoop(dwElapsedTime);
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
        __super::NotifyQuit();
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
