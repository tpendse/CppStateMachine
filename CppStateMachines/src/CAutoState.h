#pragma once

#include "FSMInterfaces.h"
#include <exception>
#include <map>

#pragma region AUTO STATES

namespace FSM
{
	namespace ___IMPL___
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		class CAutoState : FSM_STATE(TTrigger, TState)
		{
		private:
			ICallback* m_pOnEntryCallbackInstance;
			ICallback* m_pOnExitCallbackInstance;
			state_change_callback m_pOnEntryCallback;
			state_change_callback m_pOnExitCallback;

			std::map<TTrigger, TState> m_triggerStateMap;

		public:
			CAutoState(const TState& state);
			virtual ~CAutoState();

			// Implement IState interface
			virtual const TState& FindStateForTrigger(const TTrigger& trigger) override;
			virtual void OnEntry() override;
			virtual void OnExit() override;


			// Implement IStateConfigurator interface
			virtual IStateConfigurator<TTrigger, TState>* AddTrigger(const TTrigger& trigger, const TState& toState) override;

			virtual IStateConfigurator<TTrigger, TState>* OnEntry(ICallback* onEntryCallback) override;
			virtual IStateConfigurator<TTrigger, TState>* OnEntry(state_change_callback onEntryCallback) override;

			virtual IStateConfigurator<TTrigger, TState>* OnExit(ICallback* onExitCallback) override;
			virtual IStateConfigurator<TTrigger, TState>* OnExit(state_change_callback onExitCallback) override;
			virtual IState<TTrigger, TState>* State() override { return this; }

		private:

			void Call(ICallback* callback) { if (callback != nullptr) callback->Call(); }
			void Call(state_change_callback callback) { if (callback != nullptr) callback(); }
		};
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		CAutoState<TTrigger, TState>::CAutoState(const TState& state) 
			: IState<TTrigger, TState>(state, true),
			m_pOnEntryCallback(nullptr), m_pOnExitCallback(nullptr)
		{
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		CAutoState<TTrigger, TState>::~CAutoState()
		{
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		inline const TState& CAutoState<TTrigger, TState>::FindStateForTrigger(const TTrigger& trigger)
		{
			typename std::map<TTrigger, TState>::const_iterator itr = m_triggerStateMap.find(trigger);
			if (itr != m_triggerStateMap.end())
			{
				return itr->second;
			}

			throw std::exception("Cannot find the state!");
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		void CAutoState<TTrigger, TState>::OnEntry()
		{
			Call(m_pOnEntryCallback);
			Call(m_pOnEntryCallbackInstance);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		void CAutoState<TTrigger, TState>::OnExit()
		{
			Call(m_pOnExitCallback);
			Call(m_pOnExitCallbackInstance);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::AddTrigger(const TTrigger & trigger, const TState & toState)
		{
			typename std::map<TTrigger, TState>::const_iterator itr = m_triggerStateMap.find(trigger);
			if (itr == m_triggerStateMap.end())
			{
				m_triggerStateMap.insert(std::make_pair(trigger, toState));
			}

			return this;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::OnEntry(ICallback* onEntryCallback)
		{
			// Throw if already subscribed, to avoid memory leaks
			if (m_pOnEntryCallbackInstance != nullptr) throw new std::exception("Already subscribed to instance!");
			m_pOnEntryCallbackInstance = onEntryCallback;
			return this;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::OnEntry(state_change_callback onEntryCallback)
		{
			// Throw if already subscribed to instance
			if (m_pOnEntryCallbackInstance != nullptr) throw new std::exception("Already subscribed to instance!");
			m_pOnEntryCallback = onEntryCallback;
			return this;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::OnExit(ICallback* onExitCallback)
		{
			// Throw if already subscribed, to avoid memory leaks
			if (m_pOnExitCallbackInstance != nullptr) throw new std::exception("Already subscribed to instance!");
			m_pOnExitCallbackInstance = onExitCallback;
			return this;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::OnExit(state_change_callback onExitCallback)
		{
			// Throw if already subscribed to instance
			if (m_pOnExitCallbackInstance != nullptr) throw new std::exception("Already subscribed to instance!");
			m_pOnExitCallback = onExitCallback;
			return this;
		}
	}
}

#pragma endregion