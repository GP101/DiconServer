#include "KMyUserManager.h"
#include "NetError.h"
#include <functional>


CONSTRUCTOR KMyUserManager::KMyUserManager()
{
}

DESTRUCTOR KMyUserManager::~KMyUserManager()
{
}

VIRTUAL bool KMyUserManager::Initialize( DWORD dwParam_ )
{
    KUserManager::Initialize( dwParam_ );
    return true;
}

VIRTUAL void KMyUserManager::Finalize()
{
    KUserManager::Finalize();
}

bool KMyUserManager::InsertUser( KMyUserPtr spUser_ )
{
    if( spUser_ == nullptr )
        return false;

    CSLOCK( m_csUser )
    {
        m_vecUser.push_back( spUser_ );
        m_mapUidUser.insert( std::make_pair( spUser_->GetUid(), spUser_ ) );
    }

    m_mapCharacterId.insert( std::make_pair( spUser_->GetCharUid(), spUser_) );
    return true;
}

VIRTUAL bool KMyUserManager::DeleteUser( const LONGLONG nUid_ )
{
    KMyUserPtr spUser;
    GetUser( nUid_, OUT spUser );

    if( spUser == nullptr ) {
        BEGIN_LOG( cerr, L"User not found" )
            << LOG_NAMEVALUE( nUid_ )
            << LOG_NAMEVALUE( m_mapUidUser.size() )
            << LOG_NAMEVALUE( m_mapCharacterId.size() );
        return false;
    }

    m_mapCharacterId.erase( spUser->GetCharUid() );

    return KUserManager::DeleteUser( nUid_ );
}

bool KMyUserManager::DeleteByCharacterId( const LONGLONG nCid_ )
{
    KMyUserPtr spUser( GetByCharacterId( nCid_ ) );

    if( spUser == nullptr ) {
        BEGIN_LOG( cerr, L"No Character Id" )
            << LOG_NAMEVALUE( nCid_ )
            << LOG_NAMEVALUE( m_mapUidUser.size() )
            << LOG_NAMEVALUE( m_mapCharacterId.size() );
        return false;
    }

    return DeleteUser( spUser->GetUid() );
}

KMyUserPtr KMyUserManager::GetByCharacterId( const LONGLONG nCid_ )
{
    auto mit = m_mapCharacterId.find( nCid_ );

    if( mit == m_mapCharacterId.end() )
        return KMyUserPtr();

    return mit->second;
}

KMyUserPtr KMyUserManager::GetByCharacterName( const std::wstring& strCharName_ )
{
    auto mit = m_mapCharacterName.find( strCharName_ );

    if( mit == m_mapCharacterName.end() )
        return KMyUserPtr();

    return mit->second;
}

void KMyUserManager::RegisterCharInfo( IN KMyUser& kUser_ )
{
    m_mapCharacterId.insert( std::make_pair( kUser_.GetCharUid(), kUser_.GetSharedPtr<KMyUser>() ) );
    m_mapCharacterName.insert( std::make_pair( kUser_.GetCharName(), kUser_.GetSharedPtr<KMyUser>() ) );
}

void KMyUserManager::UnregisterCharInfo( IN KMyUser& kUser_ )
{
    m_mapCharacterId.erase( kUser_.GetCharUid() );
    m_mapCharacterName.erase( kUser_.GetCharName() );
}

bool KMyUserManager::QueueingByCharacterId( const LONGLONG nReceiverCid_, const KPacketPtr& spPacket_ )
{
    KMyUserPtr spUser( GetByCharacterId( nReceiverCid_ ) );

    if( spUser == nullptr ) {
        BEGIN_LOG( cerr, L"error" ) << END_LOG;
        return false;
    }

    spUser->QueueingPacket( spPacket_ );
    return true;
}
