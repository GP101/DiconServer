#include "KMyUser.h"
#include "KGen.h"
#include "KMyUserFsm.h"
#include "KDbLayer.h"
#include "NetError.h"
#include "KNetLayer.h"
#include "KUserManager.h"
#include "KMyUserManager.h"
#include <functional>


KMyUser::KMyUser()
{
    m_nCharacterUid = 0;
    m_iCharacterLevel = 0;

    m_iExp = 0;
    m_iGamePoint = 0;
}

KMyUser::~KMyUser()
{
}

VIRTUAL void KMyUser::OnPacket( IN KPacket* pkPacket_ )
{
    BEGIN_LOG( clog, L"[r] " )
        << pkPacket_->GetIdWstr()
        << L" (name:" << m_wstrName << L")"
        << END_LOG;

    DWORD dwElapTime = ::GetTickCount();

    switch( pkPacket_->m_usPacketId ) {
        CASE_PACKET( ECLGS_VERIFY_ACCOUNT_REQ, KECLGS_VERIFY_ACCOUNT_REQ );
        CASE_PACKET( EGSCN_CHECK_UUID_ACK, KECLGS_VERIFY_ACCOUNT_REQ );
        CASE_PACKET( EGSCN_USER_AUTHENTICATE_ACK, KECLGS_VERIFY_ACCOUNT_ACK );
        CASE_PACKET( EGSCL_KICK_USER_NOT, KIdPacket );
        CASE_PACKET( ECLGS_USER_CHAT, KECLGS_USER_CHAT );
        CASE_PACKET( ECLGS_P2PINFO_NOT, KECLGS_P2PINFO_NOT );
        CASE_PACKET( EDB_UPDATE_USER_INFO_FAILED_ACK, KEDB_UPDATE_USER_INFO_FAILED_ACK );

        default:
            BEGIN_LOG( cerr, L"Packet handler is not defined - " << pkPacket_->GetIdWstr() )
                << LOG_NAMEVALUE( pkPacket_->m_usPacketId );
    }//switch

    dwElapTime = ::GetTickCount() - dwElapTime;
    if( dwElapTime > 3000 ) {
        BEGIN_LOG( cwarn, L"" )
            << LOG_NAMEVALUE( pkPacket_->GetIdWstr() )
            << LOG_NAMEVALUE( pkPacket_->m_usPacketId )
            << LOG_NAMEVALUE( dwElapTime );
    }
}

void KMyUser::DbUpdateCharInfo()
{
    KEDB_UPDATE_USER_INFO_REQ kReq;
    kReq.m_iCharacterLevel = m_iCharacterLevel;
    kReq.m_iExp = m_iExp;
    kReq.m_iGamePoint = m_iGamePoint;

    m_iExp = m_iExp;
    m_iGamePoint = m_iGamePoint;

    NetSendTo( SNDTO_DB, GetCharUid(), EDB_UPDATE_USER_INFO_REQ, kReq );
    m_kTimer[TM_DB_UPDATE].restart();
}

VIRTUAL void KMyUser::Update(DWORD dwElapsedTime_)
{
    VIRTUAL KUser::Update(dwElapsedTime_);

    const DWORD dbUpdateTime = 60 * 20;
    // update Db
    if( m_kTimer[TM_DB_UPDATE].elapsed() >= dbUpdateTime ) {
        const bool isCheckState = m_pkCurrentState->CheckState( 4
            , KMyUserFsm::STATE_INIT, KMyUserFsm::STATE_LOGIN, KMyUserFsm::STATE_EXIT, KMyUserFsm::STATE_CHARACTER_LIST );
        if( isCheckState == true ) {
            m_kTimer[TM_DB_UPDATE].restart();
        }
        else {
            DbUpdateCharInfo();
        }
    }

    switch( GetStateId() ) {
    case KMyUserFsm::STATE_EXIT:
        NetSendTo( SNDTO_CENTER, GetUid(), EGSCN_USER_DISCONNECT_NOT );
        _KUserManager->RegisterDeleteUser( m_nUid );
        break;
    }
}

VIRTUAL void KMyUser::OnDestroy()
{
    KSession::OnDestroy();

    KMyUserPtr spUser = GetSharedPtr<KMyUser>();
    
    switch( GetStateId() ) {
    case KMyUserFsm::STATE_EXIT:
        return; // already in destruction state

    case KMyUserFsm::STATE_INIT:
    case KMyUserFsm::STATE_LOGIN:
    case KMyUserFsm::STATE_CHARACTER_LIST:
        StateTransition( KMyUserFsm::INPUT_EXIT );
        break;

    case KMyUserFsm::STATE_FIELD:
    case KMyUserFsm::STATE_ROOM:
    case KMyUserFsm::STATE_PLAYING:
        DbUpdateCharInfo();
        // Note: at this point, we are inside in Update()
        StateTransition( KMyUserFsm::INPUT_EXIT );
        break;
    }
}

void KMyUser::On_ECLGS_VERIFY_ACCOUNT_REQ( LONGLONG nSenderUid_, KECLGS_VERIFY_ACCOUNT_REQ& kPacket_ )
{
    if( CheckState( 1, KMyUserFsm::STATE_INIT ) == false ) {
        BEGIN_LOG( cerr, L"error" )
            << LOG_NAMEVALUE( m_wstrName )
            << L"    Current State : " << KMyUserFsm::EUserStateToString( GetStateId() )
            << END_LOG;
        return;
    }

    BEGIN_LOG( cout, L"" )
        << LOG_NAMEVALUE( kPacket_.m_wstrUserId )
        << LOG_NAMEVALUE( kPacket_.m_wstrPassword );

    NetSendTo( SNDTO_REMOTE_DB, m_nUid, EGSCN_CHECK_UUID_REQ, kPacket_ );
}

void KMyUser::On_EGSCN_CHECK_UUID_ACK( LONGLONG nSenderUid_, KECLGS_VERIFY_ACCOUNT_REQ& kPacket_ )
{
    if( CheckState( 1, KMyUserFsm::STATE_INIT ) == false ) {
        BEGIN_LOG( cerr, L"error" )
            << LOG_NAMEVALUE( m_wstrName )
            << L"    Current State : " << KMyUserFsm::EUserStateToString( GetStateId() )
            << END_LOG;
        return;
    }

    if( kPacket_.m_nUid == 0 ) {
        BEGIN_LOG( cerr, L"" )
            << LOG_NAMEVALUE( kPacket_.m_wstrUserId );
        ReserveDestroy();
        return;
    }

    // check duplicate connection
    KUserPtr spUser( _KUserManager->GetUser( kPacket_.m_nUid ) );

    if( spUser != nullptr ) {
        BEGIN_LOG( cerr, "duplicate connection" );

        KECLGS_VERIFY_ACCOUNT_ACK kPacket;
        kPacket.m_nOk = NetError::GetLastError();

        SendPacket( ECLGS_VERIFY_ACCOUNT_ACK, kPacket );
        return;
    }

    // update Uid
    _KUserManager->UpdateUid( kPacket_.m_nUid, *this );

    SetName( kPacket_.m_wstrUserId );

    // need to check duplication and etc.
    NetSendTo( SNDTO_CENTER, m_nUid, EGSCN_USER_AUTHENTICATE_REQ, kPacket_ );
}

void KMyUser::On_EGSCN_USER_AUTHENTICATE_ACK( LONGLONG nSenderUid_, KECLGS_VERIFY_ACCOUNT_ACK& kPacket_ )
{
    if( CheckState( 1, KMyUserFsm::STATE_INIT ) == false ) {
        BEGIN_LOG( cerr, L"error" )
            << LOG_NAMEVALUE( m_wstrName )
            << L"    Current State : " << KMyUserFsm::EUserStateToString( GetStateId() )
            << END_LOG;
        return;
    }

    SendPacket( ECLGS_VERIFY_ACCOUNT_ACK, kPacket_ );

    BEGIN_LOG( cout, L"" )
        << LOG_NAMEVALUE( kPacket_.m_nOk )
        << LOG_NAMEVALUE( GetUid() )
        << LOG_NAMEVALUE( GetName() )
        << LOG_NAMEVALUE( kPacket_.m_nUserUid )
        << LOG_NAMEVALUE( kPacket_.m_nOk )
        << LOG_NAMEVALUE( NetError::GetErrorStr( kPacket_.m_nOk ) );

    if( kPacket_.m_nOk != NetError::NET_OK ) {
        return;
    }

    StateTransition( KMyUserFsm::INPUT_AUTHENTICATED );
    StateTransition( KMyUserFsm::INPUT_MOVE_TO_CHARACTER_LIST );  // skip STATE_LOGIN state
}

void KMyUser::On_ECLGS_USER_CHAT( LONGLONG nSenderUid_, KECLGS_USER_CHAT& kPacket_ )
{
}

void KMyUser::On_EGSCL_KICK_USER_NOT( LONGLONG nSenderUid_, KIdPacket& kPacket_ )
{
    BEGIN_LOG( cout, L"" )
        << LOG_NAMEVALUE( KMyUserFsm::EUserStateToString( GetStateId() ) )
        << LOG_NAMEVALUE( kPacket_.m_id );

    SendPacket( EGSCL_KICK_USER_NOT, kPacket_ );

    ReserveDestroy();
}

void KMyUser::On_ECLGS_LEAVE_PLAYER_REQ( LONGLONG nSenderUid_, KIdPacket& kPacket_ )
{
}

void KMyUser::On_ECLGS_P2PINFO_NOT( LONGLONG nSenderUid_, KECLGS_P2PINFO_NOT& kPacket_ )
{
    m_vecIp     = kPacket_.m_vecIp;
    m_vecPort   = kPacket_.m_vecPort;

    BEGIN_LOG( cout, "" )
        << LOG_NAMEVALUE( kPacket_.m_vecIp.size() )
        << LOG_NAMEVALUE( kPacket_.m_vecPort.size() );
}

void KMyUser::On_EDB_UPDATE_USER_INFO_FAILED_ACK( LONGLONG nSenderUid_, KEDB_UPDATE_USER_INFO_FAILED_ACK& kPacket_ )
{
    BEGIN_LOG( cerr, L"" )
        << LOG_NAMEVALUE( m_nCharacterUid )
        << LOG_NAMEVALUE( m_wstrCharacterName )
        << LOG_NAMEVALUE( kPacket_.m_iExp )
        << LOG_NAMEVALUE( kPacket_.m_iGamePoint )
        << LOG_NAMEVALUE( m_iExp )
        << LOG_NAMEVALUE( m_iGamePoint );

    m_iExp = kPacket_.m_iExp;
    m_iGamePoint = kPacket_.m_iGamePoint;
}

void KMyUser::AddExp( int iExp )
{
    m_iExp += iExp;

    if( m_iExp < m_iTargetExpToLevelUp )
        return;

    while( m_iExp >= m_iTargetExpToLevelUp ) {
        m_iCharacterLevel++; // level up
        m_iExp -= m_iTargetExpToLevelUp;
        m_iTargetExpToLevelUp = 0; // set exp here jintaeks on 2017-08-23_10-07
    }

    BEGIN_LOG( cout, L"Level up : " << m_wstrCharacterName )
        << END_LOG;
    // Send LevelUp Msg
}

void KMyUser::AddGamePoint( int iGamePoint_ )
{
    m_iGamePoint += iGamePoint_;
}
