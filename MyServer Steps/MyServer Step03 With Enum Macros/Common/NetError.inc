#ifdef _NOT_DEFINED_
enum
{
#endif

    _ENUM( NET_OK = 0, success )
    _ENUM( ERR_UNKNOWN, unknown error )

    // ACCOUNT VERIFICATION
    _ENUM( ERR_VERIFY_00, duplicate login for a server )
    _ENUM( ERR_VERIFY_01, duplicate login for different servers )
    _ENUM( ERR_VERIFY_02, failed of Ip verification )
    _ENUM( ERR_VERIFY_03, duplicate Uid in server )
    _ENUM( ERR_CHAR_00, failed in character creation )

    _ENUM( ERR_SENTINEL, ERR_SENTINEL )

#ifdef _NOT_DEFINED_
};
#endif
