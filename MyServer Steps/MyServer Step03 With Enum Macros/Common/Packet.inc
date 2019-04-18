#ifdef _NOT_DEFINED_
enum
{
#endif

    _ENUM( ECLGS_HEART_BIT = 0 )
    _ENUM( EGSCL_ACCEPT_CONNECTION_NOT )    // server -> client connection
    _ENUM( EDB_ON_START_REQ )           // for onetime Db processing when server initialization
    _ENUM( EDB_ON_START_ACK )
    _ENUM( EDB_PERIODIC_UPDATE )          // periodic Db update
    _ENUM( EGSCN_DIRECT_TO_CENTER_DB )
    _ENUM( EGSCN_DIRECT_TO_USER )           // reserved for remote user and remote server
    _ENUM( EGSCN_DIRECT_TO_CENTER )
    _ENUM( EGSCL_KICK_USER_NOT )
    _ENUM( ECNGS_CONNECTION_LOST_NOT )
    _ENUM( EGSCN_DIRECT_TO_RSERVER )

    _ENUM( EGSCN_VERIFY_ACCOUNT_REQ )
    _ENUM( EGSCN_VERIFY_ACCOUNT_ACK )

    _ENUM( ECLGS_VERIFY_ACCOUNT_REQ )
    _ENUM( ECLGS_VERIFY_ACCOUNT_ACK )

    _ENUM( EGSCN_CHECK_UUID_REQ ) // game server to center server: ask Uid
    _ENUM( EGSCN_CHECK_UUID_ACK )

    _ENUM( EGSCN_USER_AUTHENTICATE_REQ ) // game server to center server: ask authentification
    _ENUM( EGSCN_USER_AUTHENTICATE_ACK )

    _ENUM( EGSCN_USER_DISCONNECT_NOT ) // game server to center server: notify user disconnection
    _ENUM( EDB_UPDATE_USER_INFO_REQ )
    _ENUM( EDB_UPDATE_USER_INFO_FAILED_ACK )

    _ENUM( ECLGS_USER_CHAT )

    _ENUM( ECLGS_LEAVE_PLAYER_REQ )
    _ENUM( ECLGS_LEAVE_PLAYER_NOT )

    _ENUM( ECLGS_P2PINFO_NOT ) // test for p2p connection

    _ENUM( PACKETID_SENTINEL ) // end of packets

#ifdef _NOT_DEFINED_
};
#endif
