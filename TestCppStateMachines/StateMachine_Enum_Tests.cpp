#include "stdafx.h"

#include <catch2\catch.hpp>

#include "export.h"
#include "Fakes.h"

using namespace FSM;
using namespace Fakes;


TEST_CASE("State Machine - Adding States")
{
	CREATE_FSM(fsm, TestState1);
	FakeState state(TestState3);


	//:::::::::::::::::::::: Auto states ::::::::::::::::::::::


	SECTION("Adding auto state, state added")
	{
		auto* config = fsm.Configure(TestState3);

		REQUIRE(config != nullptr);
	}


	//:::::::::::::::::::::: Custom states ::::::::::::::::::::::


	SECTION("Adding custom state, state added")
	{
		const bool result = fsm.AddState(TestState3, &state);

		REQUIRE(result);
	}

	SECTION("Adding a custom state which already exists, state not added")
	{
		FakeState duplicate(TestState1);
		fsm.AddState(TestState3, &state);
		const bool result = fsm.AddState(TestState3, &duplicate);

		REQUIRE(result == false);
	}
}








TEST_CASE("State Machine - Configuring States")
{
	CREATE_FSM(fsm, TestState1);


	//:::::::::::::::::::::: Auto states ::::::::::::::::::::::


	SECTION("Configure state transition, succeeds")
	{
		fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);
	}
}








TEST_CASE("State Machine - Firing")
{
	CREATE_FSM(fsm, TestState1);

	fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);


	SECTION("Fire uninitialized trigger, throws")
	{
		REQUIRE_THROWS( fsm.Fire(TestTrigger1));
	}

	SECTION("Fire initialized trigger, changes state")
	{
		fsm.Configure(TestState2);
		fsm.Fire(TestTrigger1);

		REQUIRE(*fsm.CurrentState() == TestState2);
	}

	SECTION("Fire unregistered trigger, throws")
	{
		REQUIRE_THROWS(fsm.Fire(TestTrigger3));
	}
}








TEST_CASE("State Machine - Callbacks - functors")
{
	CREATE_FSM(fsm, TestState1);
	fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);

	SECTION("Adding callbacks")
	{
		FakeCallback onExitCallback, onEntryCallback;
		fsm.Configure(TestState2)->OnEntry(&onEntryCallback);
		fsm.Configure(TestState1)->OnExit(&onExitCallback);

		REQUIRE(onEntryCallback.CallbackCount == 0);
		REQUIRE(onExitCallback.CallbackCount == 0);

		SECTION("Fire trigger, current state on-exit notified")
		{
			fsm.Fire(TestTrigger1);
			
			REQUIRE(onExitCallback.CallbackCount == 1);
		}

		SECTION("Fire trigger, next state on-entry notified")
		{
			fsm.Fire(TestTrigger1);

			REQUIRE(onEntryCallback.CallbackCount == 1);
		}
	}
}








TEST_CASE("State Machine - chaining & fluent interface- functors")
{
	CREATE_FSM(fsm, TestState1);
	fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);

	SECTION("Chaining callbacks works")
	{
		FakeCallback onExitCallback, onEntryCallback;
		fsm.Configure(TestState2)
			->OnEntry(&onEntryCallback)
			->OnExit(&onExitCallback)
			->AddTrigger(TestTrigger2, TestState1);

		REQUIRE(onEntryCallback.CallbackCount == 0);
		REQUIRE(onExitCallback.CallbackCount == 0);
	}
}








class TestCallbackMarshal
{
public:

	static int EntryCallbackCount;
	static int ExitCallbackCount;

	static void EntryCallback() { ++EntryCallbackCount; }
	static void ExitCallback() { ++ExitCallbackCount; }

	static void Reset()
	{
		EntryCallbackCount = 0;
		ExitCallbackCount = 0;
	}
};

int TestCallbackMarshal::EntryCallbackCount = 0;
int TestCallbackMarshal::ExitCallbackCount = 0;


TEST_CASE("State Machine - Callbacks - func ptrs")
{
	CREATE_FSM(fsm, TestState1);
	fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);

	SECTION("Adding callbacks")
	{
		//FakeCallback onExitCallback, onEntryCallback;
		fsm.Configure(TestState2)->OnEntry(&TestCallbackMarshal::EntryCallback);
		fsm.Configure(TestState1)->OnExit(&TestCallbackMarshal::ExitCallback);

		REQUIRE(TestCallbackMarshal::EntryCallbackCount == 0);
		REQUIRE(TestCallbackMarshal::ExitCallbackCount == 0);

		SECTION("Fire trigger, current state on-exit notified")
		{
			fsm.Fire(TestTrigger1);

			REQUIRE(TestCallbackMarshal::ExitCallbackCount == 1);

			TestCallbackMarshal::Reset();
		}

		SECTION("Fire trigger, next state on-entry notified")
		{
			fsm.Fire(TestTrigger1);

			REQUIRE(TestCallbackMarshal::EntryCallbackCount == 1);

			TestCallbackMarshal::Reset();
		}
	}
}








TEST_CASE("State Machine - chaining & fluent interface - func ptrs")
{
	CREATE_FSM(fsm, TestState1);
	fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);

	SECTION("Chaining callbacks works")
	{
		FakeCallback onExitCallback, onEntryCallback;
		fsm.Configure(TestState2)
			->OnEntry(&TestCallbackMarshal::EntryCallback)
			->OnExit(&TestCallbackMarshal::ExitCallback)
			->AddTrigger(TestTrigger2, TestState1);

		REQUIRE(TestCallbackMarshal::EntryCallbackCount == 0);
		REQUIRE(TestCallbackMarshal::ExitCallbackCount == 0);

		TestCallbackMarshal::Reset();
	}
}








unsigned int entryCallbackCount1 = 0, entryCallbackCount2 = 0, exitCallbackCount1 = 0, exitCallbackCount2 = 0;

void EntryCallback1()	{ ++entryCallbackCount1; }
void EntryCallback2()	{ ++entryCallbackCount2; }
void ExitCallback1()	{ ++exitCallbackCount1; }
void ExitCallback2()	{ ++exitCallbackCount2; }

void AssertEntryCallbackCounts(
	const unsigned int entry1 = 0,
	const unsigned int entry2 = 0,
	const unsigned int exit1 = 0,
	const unsigned int exit2 = 0)
{
	REQUIRE(entryCallbackCount1 == entry1);
	REQUIRE(entryCallbackCount2 == entry2);
	REQUIRE(exitCallbackCount1 == exit1);
	REQUIRE(exitCallbackCount2 == exit2);
}

void ResetCount()
{
	entryCallbackCount1 = 0;
	entryCallbackCount2 = 0;
	exitCallbackCount1 = 0;
	exitCallbackCount2 = 0;
}


TEST_CASE("State Machine - resubscribing to event - func ptrs")
{
	GIVEN("State machine created")
	{
		ResetCount();
		CREATE_FSM(fsm, TestState1);
		auto state1Configurator = fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);
		auto state2Configurator = fsm.Configure(TestState2)->AddTrigger(TestTrigger2, TestState1);

		WHEN("Subscribing on exit multiple times")
		{
			state1Configurator->OnExit(&ExitCallback1);
			state1Configurator->OnExit(&ExitCallback2);

			THEN("Pointer replaced") { AssertEntryCallbackCounts(0, 0, 0, 0); }

			AND_WHEN("Called")
			{
				fsm.Fire(TestTrigger1);

				THEN("Last subscribed is called") { AssertEntryCallbackCounts(0, 0, 0, 1); }
			}
		}

		WHEN("Subscribing on entry multiple times")
		{
			state2Configurator->OnEntry(&EntryCallback1);
			state2Configurator->OnEntry(&EntryCallback2);

			THEN("Pointer replaced") { AssertEntryCallbackCounts(0, 0, 0, 0); }

			AND_WHEN("Called")
			{
				fsm.Fire(TestTrigger1);

				THEN("Last subscribed is called") { AssertEntryCallbackCounts(0, 1, 0, 0); }
			}
		}

		AND_GIVEN("Subscribed to functors already")
		{
			FakeCallback callbackInstance1, callbackInstance2;
			state1Configurator->OnExit(&callbackInstance1);
			state2Configurator->OnEntry(&callbackInstance2);

			WHEN("Subscribing to entry with function pointer")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state2Configurator->OnEntry(&EntryCallback1));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}

			WHEN("Subscribing to exit with function pointer")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state1Configurator->OnExit(&ExitCallback1));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}
		}
	}
}








TEST_CASE("State Machine - resubscribing to event - functors")
{
	GIVEN("State machine created")
	{
		ResetCount();
		CREATE_FSM(fsm, TestState1);
		auto state1Configurator = fsm.Configure(TestState1)->AddTrigger(TestTrigger1, TestState2);
		auto state2Configurator = fsm.Configure(TestState2)->AddTrigger(TestTrigger2, TestState1);

		WHEN("Subscribing on entry")	
		{
			FakeCallback callbackInstance1, callbackInstance2;
			state1Configurator->OnEntry(&callbackInstance1);
			
			AND_WHEN("Subscribing to another functor")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state1Configurator->OnEntry(&callbackInstance2));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}

			AND_WHEN("Subscribing to same functor")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state1Configurator->OnEntry(&callbackInstance1));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}

			AND_WHEN("Subscribing to another function pointer")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state1Configurator->OnEntry(&EntryCallback1));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}
		}

		WHEN("Subscribing on exit")
		{
			FakeCallback callbackInstance1, callbackInstance2;
			state2Configurator->OnExit(&callbackInstance1);

			AND_WHEN("Subscribing to another functor")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state2Configurator->OnExit(&callbackInstance2));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}

			AND_WHEN("Subscribing to same functor")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state2Configurator->OnExit(&callbackInstance1));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}

			AND_WHEN("Subscribing to another function pointer")
			{
				THEN("Throws exception")
				{
					REQUIRE_THROWS(state2Configurator->OnExit(&EntryCallback1));

					AND_THEN("Nothing called")
					{
						REQUIRE(callbackInstance1.CallbackCount == 0);
						REQUIRE(callbackInstance2.CallbackCount == 0);
						AssertEntryCallbackCounts();
					}
				}
			}
		}
	}
}