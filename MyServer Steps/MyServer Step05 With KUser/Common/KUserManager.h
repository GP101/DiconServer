#pragma once
#include "KUser.h"
#include <map>
#include "KGen.h"
#include "KCriticalSection.h"
#include "noncopyable.h"
#include "KBaseSingleton.h"
#include "LayerUtil.h"
#include <memory>


class KUserManager
    : public noncopyable
    , public KBaseSingleton<KUserManager>
{
    friend class KBaseSingleton<KUserManager>;

protected:
    CONSTRUCTOR         KUserManager();
public:
    virtual             ~KUserManager();

    virtual bool        Initialize( DWORD dwParam_ );
    virtual void        Finalize();

    KUserPtr            GetUser( const LONGLONG nUid );
    int                 GetCount() const 
                        {
                            KCriticalSectionLock lock( m_csUser );
                            return (int)m_mapUidUser.size(); 
                        }

    template<typename T>
    bool                GetUser( IN const LONGLONG nUid, IN OUT std::shared_ptr<T>& spOut );
    template<typename T> 
    void                ForEach( T& functor );
    template<typename T> 
    void                SendToAll( const unsigned short usPacketId, const T& data );
    template<typename T> 
    void                QueueingToAll( const unsigned short usPacketId, const T& data );
    bool                SendTo( const LONGLONG nReceiverUid, IN const KPacket& packet );
    void                QueueingTo( const LONGLONG nReceiverUid, const KPacketPtr& spPacket );

    virtual bool        QueueingByCharacterId( const LONGLONG nReceiverCid_, const KPacketPtr& spPacket_ )
                        {
                            return false;
                        }

    KUserPtr            CreateUser();
    void                RegisterDeleteUser( const LONGLONG nUid );
    void                RegisterDeleteUserAll();
    void                Update(DWORD dwElapsedTime_);
    bool                UpdateUid( const LONGLONG nUid, KUser& kUser );

protected:
    virtual bool        DeleteUser( const LONGLONG nUid );

    KCriticalSection    m_csUser; // for m_vecUser and m_mapUidUser
    std::vector<KUserPtr>
                        m_vecUser;
    std::map<LONGLONG, KUserPtr>
                        m_mapUidUser;  // user Uid, KUserPtr

    KCriticalSection    m_csDeletion;
    std::vector<LONGLONG>
                        m_vecDeletionCandidate;

    CreateUserCallback  m_pfnCreateUser;
    CreateFsmCallback   m_pfnCreateFsm;
};

template<typename UserType>
bool KUserManager::GetUser( IN const LONGLONG nUid_, IN OUT std::shared_ptr<UserType>& spOut_ )
{
    KUserPtr spUser( GetUser( nUid_ ) );

    if( spUser == nullptr )
        return false;

    // do not implement custom Rtti just use C++ builtin Rtti.
    // jintaeks on 2017-09-08_18-02
    if( dynamic_cast<UserType*>( spUser.get() ) == nullptr )
        return false;

    spOut_ = std::static_pointer_cast<UserType>( spUser );
    return true;
}

template<typename Func> 
void KUserManager::ForEach( Func& functor )
{
    KCriticalSectionLock lock( m_csUser );
    std::for_each( m_vecUser.begin(), m_vecUser.end(), functor );
}

template<typename T>
void KUserManager::QueueingToAll( const unsigned short usPacketId_, const T& data_ )
{
    KPacketPtr spPacket( new KPacket );
    spPacket->SetData( 0, usPacketId_, data_ );

    CSLOCK( m_csUser )
    {
        std::vector<KUserPtr>::iterator vit;
        for( vit = m_vecUser.begin(); vit != m_vecUser.end(); ++vit ) {
            ( *vit )->QueueingPacket( spPacket );
        }
    }
}

template<typename T>
void KUserManager::SendToAll( const unsigned short usPacketId_, const T& data_ )
{
    KCriticalSectionLock lock( m_csUser );

    std::vector<KUserPtr>::iterator vit;
    for( vit = m_vecUser.begin(); vit != m_vecUser.end(); ++vit ) {
        ( *vit )->SendPacket( usPacketId_, data_ );
    }
}

#define _KUserManager   KUserManager::Instance()
