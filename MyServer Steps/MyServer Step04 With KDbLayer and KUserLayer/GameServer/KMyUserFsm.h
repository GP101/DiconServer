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
    );

    DECLARE_ENUM( EUserStateInput
        , INPUT_AUTHENTICATED
        , INPUT_EXIT
    );

                        KMyUserFsm::KMyUserFsm() : KFsm( KMyUserFsm::EUserState::STATE_INIT )
                        {
                            // STATE_INIT
                            KFsmState state0( STATE_INIT );
                            state0.AddTransition( INPUT_AUTHENTICATED, STATE_LOGIN );
                            state0.AddTransition( INPUT_EXIT, STATE_EXIT );
                            AddState( state0 );

                            // STATE_LOGIN
                            KFsmState state1( STATE_LOGIN );
                            state1.AddTransition( INPUT_EXIT, STATE_EXIT );
                            AddState( state1 );

                            // STATE_EXIT
                            KFsmState state2( STATE_EXIT );
                            AddState( state2 );
                        }
    virtual             ~KMyUserFsm() {}
};
