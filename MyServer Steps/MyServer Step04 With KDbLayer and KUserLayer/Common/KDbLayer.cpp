#include "KDbLayer.h"


CONSTRUCTOR KDbLayer::KDbLayer()
    : m_pfnCreateThread( nullptr )
{
}

DESTRUCTOR KDbLayer::~KDbLayer()
{
}

bool KDbLayer::Initialize( DWORD dwParam_ ) 
{
    KDbLayerInitParam* pInitParam = reinterpret_cast<KDbLayerInitParam*>( dwParam_ );
    BOOST_ASSERT( pInitParam != nullptr );

    m_pfnCreateThread = pInitParam->pfnCreateDbThread;

    const bool bIsInitialized = KThreadManager::Initialize( pInitParam->dwNumThreads );
    if( bIsInitialized == true )
        KThreadManager::BeginAllThread();

    return bIsInitialized;
}

void KDbLayer::Finalize()
{
    KThreadManager::EndAllThread();
    VIRTUAL KThreadManager::Finalize();
}

void KDbLayer::Dump()
{
}

VIRTUAL KThread* KDbLayer::CreateThread()
{
    if( m_pfnCreateThread != nullptr ) {
        return m_pfnCreateThread( m_strFileDSN.c_str() );
    }
    return nullptr;
}
