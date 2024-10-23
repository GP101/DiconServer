#pragma once
#include "KFsm.h"
#include "EnumToString.h"


class KMyUserFsm : public KFsm
{
public:
    DECLARE_ENUM( EUserState
        , STATE_ZERO_NO_USE // use 0 for invalid state
        , STATE_INIT // use 1 for initial state
        , STATE_LOGIN
        , STATE_EXIT
        , STATE_FIELD
        , STATE_ROOM
        , STATE_PLAYING
        , STATE_CHARACTER_LIST
        , STATE_SENTINEL
    );

    DECLARE_ENUM( EUserStateInput
        , INPUT_AUTHENTICATED
        , INPUT_EXIT
        , INPUT_MOVE_TO_FIELD
        , INPUT_MOVE_TO_ROOM
        , INPUT_MOVE_TO_PLAYING
        , INPUT_MOVE_TO_CHARACTER_LIST
    );

    CONSTRUCTOR         KMyUserFsm();
    virtual             ~KMyUserFsm() {}
};
