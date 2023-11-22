#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "KFsm.h"

#define VERIFY_STATE2( ... ) \
    if( !m_spFsm->GetCurrentState()->CheckState(PRN_ENUM_TEXT_NARG(__VA_ARGS__), __VA_ARGS__) ) \
    { \
        BEGIN_LOG( std::cerr, L"error" ) \
        << L"    Current State : " << GetStateIdString( GetStateId() ) << std::endl \
        << L"    Valid State : " L ## #__VA_ARGS__ << std::endl; \
        return; \
    }

class KNetLayer : public KFsmBase
{
    DECLARE_ENUM(ENetState,
        NET_STATE_IDLE,
        NET_STATE_MAIN
    );
public:
    CONSTRUCTOR			KNetLayer();
    void				Update();
    const char*         GetStateIdString(int iState) const
                        {
                            return ENetStateToString(iState);
                        }
};

KNetLayer::KNetLayer()
{
    KFsmState       state0(ENetState::NET_STATE_IDLE);
    KFsmState       state1(ENetState::NET_STATE_MAIN);
    KFsmPtr			spStateMachine;
    spStateMachine.reset(new KFsm(ENetState::NET_STATE_IDLE));

    state0.AddTransition(10, state1);
    state0.AddTransition(20, KFsmState::INVALID_STATE);
    spStateMachine->AddState(state0);

    state1.AddTransition(30, state0);
    spStateMachine->AddState(state1);

    SetFsm(spStateMachine);
}

void KNetLayer::Update()
{
    StateTransition(10);
    VERIFY_STATE2(ENetState::NET_STATE_IDLE);
}

void main()
{
    std::shared_ptr<KNetLayer>    spNetLayer;
    spNetLayer.reset(new KNetLayer());
    spNetLayer->Update();
}//main()
