#include "KMyUserFsm.h"
#include "NetUtil.h"


KMyUserFsm::KMyUserFsm() : KFsm( KMyUserFsm::EUserState::STATE_INIT )
{
    // STATE_INIT
    KFsmState state0( STATE_INIT );
    state0.AddTransition( INPUT_AUTHENTICATED, STATE_LOGIN );
    state0.AddTransition( INPUT_EXIT, STATE_EXIT );
    state0.AddTransition( INPUT_MOVE_TO_FIELD, STATE_FIELD );
    AddState( state0 );

    // STATE_LOGIN
    KFsmState state1( STATE_LOGIN );
    state1.AddTransition( INPUT_EXIT, STATE_EXIT );
    state1.AddTransition( INPUT_MOVE_TO_CHARACTER_LIST, STATE_CHARACTER_LIST );
    AddState( state1 );

    // STATE_CHARACTER_LIST
    KFsmState state2( STATE_CHARACTER_LIST );
    state2.AddTransition( INPUT_EXIT, STATE_EXIT );
    state2.AddTransition( INPUT_MOVE_TO_FIELD, STATE_FIELD );
    AddState( state2 );

    // STATE_FIELD
    KFsmState state3( STATE_FIELD );
    state3.AddTransition( INPUT_EXIT, STATE_EXIT );
    state3.AddTransition( INPUT_MOVE_TO_ROOM, STATE_ROOM );
    AddState( state3 );

    // STATE_ROOM
    KFsmState state4( STATE_ROOM );
    state4.AddTransition( INPUT_MOVE_TO_FIELD, STATE_FIELD );
    state4.AddTransition( INPUT_MOVE_TO_PLAYING, STATE_PLAYING );
    AddState( state4 );

    // STATE_PLAYING
    KFsmState state5( STATE_PLAYING );
    state5.AddTransition( INPUT_MOVE_TO_ROOM, STATE_ROOM );
    AddState( state5 );

    // STATE_EXIT
    KFsmState state6( STATE_EXIT );
    AddState( state6 );
}
