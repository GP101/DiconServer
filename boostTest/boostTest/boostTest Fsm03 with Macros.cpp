#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "KLog.h"
#include "KFsm.h"

class KNetLayer
{
    DECLARE_FSM;
public:
    CONSTRUCTOR			KNetLayer();
    void				Update();
};

KNetLayer::KNetLayer()
{
    KFsmState       state0(0);
    KFsmState       state1(1);
    KFsmPtr			spStateMachine;
    spStateMachine.reset(new KFsm(0));

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
    VERIFY_STATE(0, 2, 3);
}

void main()
{
    boost::shared_ptr<KNetLayer>    spNetLayer;
    spNetLayer.reset(new KNetLayer());
    spNetLayer->Update();
}//main()
