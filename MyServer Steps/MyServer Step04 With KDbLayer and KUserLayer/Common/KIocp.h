#pragma once
#define _WINSOCKAPI_
#include <windows.h>
#include "KThreadManager.h"
#include "KGlobalSingleton.h"


class KSocket;
class KIocp
    : public KThreadManager
    , public KGlobalSingleton<KIocp>
{
    friend class KIocpWorkerThread;

    using               KThreadManager::BeginAllThread;
    using               KThreadManager::EndAllThread;

public:
    enum
    {
        QUIT_IOCP_WORKER_THREAD = 1
    };

    CONSTRUCTOR         KIocp();
    virtual             ~KIocp(); 

    /// initialize and create Iocp working threads.
    virtual bool        Initialize( DWORD nThreadNum_ );

    /// quit Iocp working threads and finalize.
    virtual void        Finalize();

    bool                BindSocket( KSocket* pkSockObj_ );
    bool                PostCompletionStatus( ULONG_PTR pCompletionKey_, DWORD dwNumBytes_ = 0
                            , OVERLAPPED* pOverlapped_ = NULL );
    
    bool                GetCompletionStatus( OUT ULONG_PTR* pCompletionKey_, OUT PDWORD pdwNumBytes_
                            , OUT OVERLAPPED** ppOverlapped_, DWORD dwMilliseconds_ = INFINITE );

    void                DeleteCompletionKey( IN OUT KSocket* pkSockObj_ );
    void                OnIoCompleted( DWORD dwKey_, DWORD dwBytesTransfer_, IN OVERLAPPED* pOverlapped_ );
    void                OnClosedByLocal( DWORD dwKey_ );
    size_t              GetNumSocketObject() const;

private:
    KSocket*            _GetSockObject( DWORD dwKey_ );
    /// create KIocpWorkerThread.
    virtual KThread*    CreateThread() override;
    DWORD               GenNewCompletionKey() const;

    HANDLE              m_hIocp;
    KCriticalSection    m_csSockObj;
    std::map<DWORD, KSocket*>
                        m_mapSocketObject;
};

#define _KIocp          KIocp::Singleton()
