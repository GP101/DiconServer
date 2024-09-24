#include "KSocket.h"
#include "KIocp.h"
#include "KGen.h"
#include "KIocpWorkerThread.h"
#include <functional>
#undef max
#include <boost/random.hpp>

using namespace std::placeholders;

CONSTRUCTOR KIocp::KIocp() 
    : m_hIocp( NULL )
{ 
}

DESTRUCTOR KIocp::~KIocp()
{ 
    assert( m_hIocp == NULL );
}

bool KIocp::Initialize( DWORD nThreadNum_ )
{
    //// create Iocp
    //// create and run Iocp working threads.

    m_hIocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
    if( m_hIocp == NULL ) {
        BEGIN_LOG( cout, L"error" );
    }

    const bool isThreadInitialized = KThreadManager::Initialize( nThreadNum_ );
    if( isThreadInitialized == true ) {
        KThreadManager::BeginAllThread();
    }

    return isThreadInitialized;
}

void KIocp::Finalize()
{
    for( auto vit = m_vecThread.begin(); vit != m_vecThread.end(); ++vit ) {
        PostCompletionStatus( QUIT_IOCP_WORKER_THREAD );
    }

    VIRTUAL KThreadManager::EndAllThread();
    VIRTUAL KThreadManager::Finalize();

    if( m_hIocp != NULL ) {
        CloseHandle( m_hIocp );
        m_hIocp = NULL;
    }
}

bool KIocp::BindSocket( KSocket* pkSockObj )
{
    if( pkSockObj == NULL )
        return false;

    CSLOCK( m_csSockObj )
    {
        const DWORD dwCompletionKey = GenNewCompletionKey();
        HANDLE hPort = CreateIoCompletionPort( (HANDLE)pkSockObj->m_sock, m_hIocp, dwCompletionKey, 0 );
        if( hPort != NULL ) {
            m_mapSocketObject.insert( std::make_pair( dwCompletionKey, pkSockObj ) );
            pkSockObj->SetKey( dwCompletionKey );
            return true;
        }
    }

    return false;
}

void KIocp::DeleteCompletionKey( KSocket* pkSockObj_ )
{
    CSLOCK( m_csSockObj )
    {
        m_mapSocketObject.erase( pkSockObj_->GetKey() );
        pkSockObj_->SetKey( KSocket::SOCKEY_NULL_KEY );
    }
}

DWORD KIocp::GenNewCompletionKey() const
{
    static boost::mt19937 rng;
    static boost::uniform_int<DWORD> uint32( 1, UINT_MAX );
    static boost::variate_generator<boost::mt19937&, boost::uniform_int<DWORD> > dice( rng, uint32 );

    CSLOCK( m_csSockObj )
    {
        while( true ) {
            const DWORD dwCompletionKey = dice();
            if( m_mapSocketObject.find( dwCompletionKey ) == m_mapSocketObject.end() )
                return dwCompletionKey;
        }
    }
    return 0L;
}

bool KIocp::PostCompletionStatus( ULONG_PTR pCompletionKey_
    , DWORD dwNumBytes
    , IN OVERLAPPED* pOverlapped_ )
{
    return PostQueuedCompletionStatus( m_hIocp, dwNumBytes
        , pCompletionKey_, pOverlapped_ ) == TRUE;
}

bool KIocp::GetCompletionStatus( OUT ULONG_PTR* pCompletionKey_
    , PDWORD pdwNumBytes
    , OUT OVERLAPPED** ppOverlapped_
    , DWORD dwMilliseconds )
{
    return GetQueuedCompletionStatus( m_hIocp, pdwNumBytes
        , OUT pCompletionKey_, OUT ppOverlapped_, dwMilliseconds ) == TRUE;
}

VIRTUAL KThread* KIocp::CreateThread()
{
    return new KIocpWorkerThread();
}

void KIocp::OnClosedByLocal( IN DWORD dwKey_ )
{
    KSocket* pkSocket = _GetSockObject(dwKey_);

    if( pkSocket != nullptr ) {
        VIRTUAL pkSocket->OnCloseSocket();
    }
}

KSocket* KIocp::_GetSockObject( DWORD dwKey_ )
{
    KCriticalSectionLock lock( m_csSockObj );

    auto mit = m_mapSocketObject.find( dwKey_ );

    if( mit == m_mapSocketObject.end() ) {
        BEGIN_LOG( cout, "Cannot find socket key : " << dwKey_ )
            << LOG_NAMEVALUE( m_mapSocketObject.size() );
        return NULL;
    }

    KSocket* pSocket = mit->second;
    return pSocket;
}

void KIocp::OnIoCompleted( DWORD dwKey_, DWORD dwBytesTransferred_, IN OVERLAPPED* pOverlapped_ )
{
    if( pOverlapped_ == NULL ) {
        BEGIN_LOG( cerr, L"IOCP Error" );
        return;
    }

    KOverlapped* pkOverlapped = static_cast<KOverlapped*>( pOverlapped_ );
    KSocket* pkSocket = _GetSockObject( dwKey_ );

    if( pkSocket == NULL ) {
        BEGIN_LOG( cerr, "Cannot find IO completed socket key : " << dwKey_ )
            << LOG_NAMEVALUE( dwBytesTransferred_ );
        return;
    }

    switch( pkOverlapped->m_eIoMode ) {
    case KOverlapped::IO_RECEIVE:
        if( &pkSocket->m_ovlReceive != pkOverlapped ) {
            BEGIN_LOG( cerr, "diffrent CompletionKey. (recv) key : " << dwKey_ );
        }
        VIRTUAL pkSocket->OnReceiveCompleted( dwBytesTransferred_ );
        break;
    case KOverlapped::IO_SEND:
        if( &pkSocket->m_ovlSend != pkOverlapped ) {
            BEGIN_LOG( cerr, "diffrent CompletionKey. (send) key : " << dwKey_ );
        }
        pkSocket->OnSendCompleted( dwBytesTransferred_ );
        break;
    }
}

size_t KIocp::GetNumSocketObject() const
{ 
    KCriticalSectionLock lock( m_csSockObj );
    return m_mapSocketObject.size(); 
}
