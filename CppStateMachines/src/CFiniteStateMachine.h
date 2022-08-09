#pragma once

//#include "FSMInterfaces.h"
//#include "CAutoState.h"
//#include "CStateMap.h"

#include "export.h"

#include<map>


#pragma region STATE MACHINE

namespace FSM
{
	template<typename TTrigger, typename TState>
	class CFiniteStateMachine : public IFiniteStateMachine<TTrigger, TState>
	{
	private:
		IState<TTrigger, TState>* m_pCurrentState;
		IStateMap<TTrigger, TState>* m_pMap;

	public:
		CFiniteStateMachine(const TState& defaultState);
		virtual ~CFiniteStateMachine();

		// Inherited via IFiniteStateMachine
		virtual IStateConfigurator<TTrigger, TState>* Configure(const TState& state) override;
		virtual const TState* CurrentState() const override;
		virtual bool AddState(const TState& state, IState<TTrigger, TState>* instance) override;
		virtual void Fire(const TTrigger& trigger) override;
	};









	template<typename TTrigger, typename TState>
	CFiniteStateMachine<TTrigger, TState>::CFiniteStateMachine(const TState& defaultState)
	{
		m_pMap = new __IMPL__::CStateMap<TTrigger, TState>();
		m_pCurrentState = this->Configure(defaultState)->State();
	}

	template<typename TTrigger, typename TState>
	inline CFiniteStateMachine<TTrigger, TState>::~CFiniteStateMachine()
	{
		if (m_pMap != nullptr)
		{
			delete m_pMap;
			m_pMap = nullptr;
		}
	}

	template<typename TTrigger, typename TState>
	inline IStateConfigurator<TTrigger, TState>* CFiniteStateMachine<TTrigger, TState>::Configure(const TState & state)
	{
		if (m_pMap->Has(state))
		{
			return dynamic_cast<IStateConfigurator<TTrigger, TState>*>(m_pMap->Get(state));
		}

		___IMPL___::CAutoState<TTrigger, TState>* instance = new ___IMPL___::CAutoState<TTrigger, TState>(state);
		m_pMap->Add(state, instance);
		return instance;
	}

	template<typename TTrigger, typename TState>
	inline const TState* CFiniteStateMachine<TTrigger, TState>::CurrentState() const
	{
		if (m_pCurrentState == nullptr) throw std::exception("Current state is null! This should not happen!");

		return &m_pCurrentState->StateType;
	}

	template<typename TTrigger, typename TState>
	inline bool CFiniteStateMachine<TTrigger, TState>::AddState(const TState & state, IState<TTrigger, TState>* instance)
	{
		if (m_pMap->Has(state)) return false;

		m_pMap->Add(state, instance);
		return true;
	}

	template<typename TTrigger, typename TState>
	inline void CFiniteStateMachine<TTrigger, TState>::Fire(const TTrigger & trigger)
	{
		const TState& state = m_pCurrentState->FindStateForTrigger(trigger);

		if (!m_pMap->Has(state)) throw std::exception("Cannot find state for type");

		IState<TTrigger, TState>* target = m_pMap->Get(state);
		
		m_pCurrentState->OnExit();
		{
			m_pCurrentState = target;
		}
		m_pCurrentState->OnEntry();

	}
}


#pragma endregion