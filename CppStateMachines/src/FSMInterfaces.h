#pragma once

#include <map>

#pragma region INTERFACES

namespace FSM
{


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ICallback
	{
	public:
		virtual ~ICallback() { /* Needs to remain empty */ }

		virtual void Call() = 0;
	};

	typedef void(*state_change_callback)();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename TTrigger, typename TState>
	class IState
	{
	public:
		IState(const TState& state, bool disposable) : StateType(state), Disposable(disposable) {  }
		virtual ~IState() { /* Needs to remain empty */ }

		const bool Disposable;
		const TState StateType;

		virtual const TState& FindStateForTrigger(const TTrigger& trigger) = 0;

		virtual void OnEntry() = 0;
		virtual void OnExit() = 0;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename TTrigger, typename TState>
	class IStateConfigurator
	{
	private:


	public:
		virtual ~IStateConfigurator() { /* Needs to remain empty */ }

		virtual IStateConfigurator<TTrigger, TState>* AddTrigger(const TTrigger& trigger, const TState& toState) = 0;

		virtual IStateConfigurator<TTrigger, TState>* OnEntry(ICallback* callback) = 0;
		virtual IStateConfigurator<TTrigger, TState>* OnEntry(state_change_callback callback) = 0;

		virtual IStateConfigurator<TTrigger, TState>* OnExit(ICallback* callback) = 0;
		virtual IStateConfigurator<TTrigger, TState>* OnExit(state_change_callback callback) = 0;

		virtual IState<TTrigger, TState>* State() = 0;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename TTrigger, typename TState>
	class IStateMap
	{
	public:

		virtual ~IStateMap() { /* Needs to remain empty */ }
		
		virtual bool Add(const TState& state, IState<TTrigger, TState>* instance) = 0;

		virtual IState<TTrigger, TState>* Get(const TState& state) const = 0;

		virtual bool Has(const TState& state) const = 0;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename TTrigger, typename TState>
	class IFiniteStateMachine
	{
	public:
		virtual ~IFiniteStateMachine() { /* Needs to remain empty */ }

		virtual IStateConfigurator<TTrigger, TState>* Configure(const TState& state) = 0;

		virtual const TState* CurrentState() const = 0;

		virtual bool AddState(const TState& state, IState<TTrigger, TState>* instance) = 0;

		virtual void Fire(const TTrigger& trigger) = 0;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FSM_STATE(TRIGGER, STATE) \
public IState<TRIGGER, STATE>, \
public IStateConfigurator<TRIGGER, STATE>

}

#pragma endregion