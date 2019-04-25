#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "KFsm.h"

void main()
{
	KFsmState       state0(0);
	KFsmState       state1(1);
	KFsm            stateMachine(0);

	state0.AddTransition(10, state1);
	state0.AddTransition(20, KFsmState::INVALID_STATE);
	stateMachine.AddState(state0);

	state1.AddTransition(30, state0);
	stateMachine.AddState(state1);

	KFsmStatePtr spCurrentState = stateMachine.GetCurrentState();
	printf("state %i\r\n", spCurrentState->GetStateId()); // 0
	stateMachine.DoTransition(10); 
	spCurrentState = stateMachine.GetCurrentState();
	printf("state %i\r\n", spCurrentState->GetStateId()); // 1
	int iState = stateMachine.DoTransition(20);
	spCurrentState = stateMachine.GetCurrentState();
	printf("return %i, state %i\r\n", iState, spCurrentState->GetStateId()); // -1, 1
	stateMachine.DoTransition(30);
	spCurrentState = stateMachine.GetCurrentState();
	printf("state %i\r\n", spCurrentState->GetStateId()); // 0

	spCurrentState = stateMachine.GetCurrentState();
	if (spCurrentState != nullptr)
	{
		const bool isState = spCurrentState->CheckState(2, 0, 1);
		printf("isState %i\r\n", isState);
	}//if
}//main()
