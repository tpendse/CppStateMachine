#include "stdafx.h"

#include <catch2\catch.hpp>

using namespace FSM;
using namespace Fakes;



struct CustomTrigger
{
public:
	std::string TriggerData;
};



struct CustomState
{
public:
	std::string StateData;
};

class Callback : public FSM::ICallback
{
private:

	std::string m_message;

public:

	Callback(const std::string& message) : m_message(message) { }

	virtual void Call() override
	{
		std::cout << m_message << std::endl;
	}
};

void ExitCallback()
{
	std::cout << "Exiting!" << std::endl;
}


TEST_CASE("State machine - non-enum types")
{
	CustomState defaultState, state1, state2;
	defaultState.StateData = "Default state";
	state1.StateData = "State 1";
	state2.StateData = "State 2";

	CustomTrigger trigger1, trigger2, trigger3;
	trigger1.TriggerData = "Trigger 1";
	trigger2.TriggerData = "Trigger 2";
	trigger3.TriggerData = "Trigger 3";

	CFiniteStateMachine<CustomTrigger*, CustomState*> fsm(&defaultState);

	Callback exitCallback("Exiting!");

	fsm.Configure(&defaultState)
		->AddTrigger(&trigger1, &state1)
		->OnExit(&exitCallback);

	fsm.Configure(&state1);

	fsm.Fire(&trigger1);
	const CustomState* current = *fsm.CurrentState();
}