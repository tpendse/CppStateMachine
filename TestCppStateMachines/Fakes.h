#pragma once

//#include "FSMInterfaces.h"
#include "export.h"
#include <exception>

namespace Fakes
{
	enum TestTriggers { TestTrigger1, TestTrigger2, TestTrigger3 };

	enum TestStates { TestState1, TestState2, TestState3 };





	class FakeState : public FSM::IState<TestTriggers, TestStates>
	{
	public:
		FakeState(const TestStates& state) : IState<TestTriggers, TestStates>(state, false) {  }
		virtual ~FakeState() { /* Needs to remain empty */ }


		// Inherited via IState
		virtual void OnEntry() override
		{
		}

		virtual void OnExit() override
		{
		}

// Disable returning address of local variable or temporary, since this is a fake
#pragma warning( push )
#pragma warning( disable : 4172)
		virtual const TestStates& FindStateForTrigger(const TestTriggers& trigger) override
		{
			return TestStates::TestState1;
		}
#pragma warning( pop ) 
	};



	class FakeCallback : public FSM::ICallback
	{
	public:
		int CallbackCount = 0;

		virtual void Call() override
		{ 
			++CallbackCount; 
		}
	};

}



#define CREATE_FSM(XNAME, DEFSTATE) CFiniteStateMachine<TestTriggers, TestStates> XNAME = CFiniteStateMachine<TestTriggers, TestStates>(DEFSTATE);