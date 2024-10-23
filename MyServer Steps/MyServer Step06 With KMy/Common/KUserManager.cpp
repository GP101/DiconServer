#include "KUserManager.h"
#include "NetError.h"
#include <functional>
#include "LayerUtil.h"

using namespace std::placeholders;


CONSTRUCTOR KUserManager::KUserManager()
    : m_pfnCreateUser( nullptr )
    , m_pfnCreateFsm( nullptr )
{
    m_vecUser.reserve( 200 ); 
}

DESTRUCTOR KUserManager::~KUserManager()
{
}

VIRTUAL bool KUserManager::Initialize( DWORD dwParam_ )
{
    KUserLayerInitParam* pInitParam = reinterpret_cast<KUserLayerInitParam*>( dwParam_ );
    assert( pInitParam != nullptr );

    m_pfnCreateUser = pInitParam->pfnCreateUserCallback;
    m_pfnCreateFsm = pInitParam->pfnCreateFsmCallback;

    return true;
}

VIRTUAL void KUserManager::Finalize()
{
    assert( m_vecUser.empty() == true );
}

KUserPtr KUserManager::CreateUser()
{
    assert( m_pfnCreateUser != nullptr );
    assert( m_pfnCreateFsm != nullptr );

    if( m_pfnCreateUser == nullptr )
        return KUserPtr();

    KUserPtr spUser = m_pfnCreateUser();

    if( spUser == nullptr )
        return KUserPtr();

    VIRTUAL spUser->Initialize( 0L );

    KFsmPtr spFsm = m_pfnCreateFsm();
    spUser->SetFSM( spFsm );

    CSLOCK( m_csUser )
    {
        while( true ) {
            LONGLONG nUid = NetUtil::GetTempUid();

            if( m_mapUidUser.find( nUid ) == m_mapUidUser.end() ) {
                spUser->SetUid( nUid );
                break;
            }
        }

        m_vecUser.push_back( spUser );
        m_mapUidUser.insert( std::make_pair( spUser->GetUid(), spUser ) );
    }

    BEGIN_LOG( clog, L"Name : " << spUser->GetName()
        << ", Uid : " << spUser->GetUid() );

    return spUser;
}

void KUserManager::RegisterDeleteUser( const LONGLONG nUid_ )
{
    KCriticalSectionLock lock( m_csDeletion );
    m_vecDeletionCandidate.push_back( nUid_ );
}

void KUserManager::RegisterDeleteUserAll()
{
    for( KUserPtr spUser : m_vecUser ) {
        RegisterDeleteUser( spUser->GetUid() );
    }
}

void KUserManager::Update(DWORD dwElapsedTime_)
{
    ForEach( std::bind( &KUser::Update, _1, dwElapsedTime_) );

    CSLOCK( m_csDeletion )
    {
        if( m_vecDeletionCandidate.empty() == false ) {
            std::for_each( m_vecDeletionCandidate.begin(), m_vecDeletionCandidate.end()
                , std::bind( &KUserManager::DeleteUser, std::ref( *this ), _1 ) );
            m_vecDeletionCandidate.clear();
        }
    }
}

VIRTUAL bool KUserManager::DeleteUser( const LONGLONG nUid_ )
{
    KCriticalSectionLock lock( m_csUser );

    auto mit = m_mapUidUser.find( nUid_ );
    if( mit == m_mapUidUser.end() ) {
        BEGIN_LOG( cerr, L"User Uid not found: " )
            << LOG_NAMEVALUE( nUid_ )
            << LOG_NAMEVALUE( m_mapUidUser.size() );
        return false;
    }

    auto vit = std::find( m_vecUser.begin(), m_vecUser.end(), mit->second );

    if( vit == m_vecUser.end() ) {
        BEGIN_LOG( cerr, L"KUserPtr not found: " )
            << LOG_NAMEVALUE( nUid_ )
            << LOG_NAMEVALUE( m_vecUser.size() );
    }
    else {
        KUserPtr spUser = *vit;
        // check validity of user
        spUser->IsRefCountLessThan( 3 );
        VIRTUAL spUser->Finalize();
        m_vecUser.erase( vit );
    }

    m_mapUidUser.erase( nUid_ );
    return true;
}

KUserPtr KUserManager::GetUser( const LONGLONG nUid_ )
{
    auto mit = m_mapUidUser.find( nUid_ );

    if( mit == m_mapUidUser.end() )
        return KUserPtr();

    return mit->second;
}

bool KUserManager::UpdateUid( const LONGLONG nUid_, KUser& kUser_ )
{
    KUserPtr spUser = std::static_pointer_cast<KUser>( kUser_.shared_from_this() );

    if( spUser == nullptr ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
        return false;
    }

    // erase() returns the number of deleted nodes.
    if( m_mapUidUser.erase( spUser->GetUid() ) == 0 ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
    }

    spUser->SetUid( nUid_ );

    std::pair<std::map<LONGLONG, KUserPtr>::iterator, bool> kPair;
    kPair = m_mapUidUser.insert( std::make_pair( nUid_, spUser ) );

    return kPair.second;
}

void KUserManager::QueueingTo( const LONGLONG nReceiverUid_, const KPacketPtr& spPacket_ )
{
    KUserPtr spUser( GetUser( nReceiverUid_ ) );

    if( spUser == nullptr ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
        return;
    }

    spUser->QueueingPacket( spPacket_ );
}

bool KUserManager::SendTo( const LONGLONG nReceiverUid_, IN const KPacket& kPacket_ )
{
    KUserPtr spUser( GetUser( nReceiverUid_ ) );

    if( spUser == nullptr )
        return false;

    return spUser->SendPacket( kPacket_ );
}
