#pragma once

#include <map>
#include <exception>

/****************************************************************************************************************************/

#pragma region INTERFACES

namespace FSM
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class ICallback
	{
	public:
		virtual ~ICallback() { /* Needs to remain empty */ }

		virtual void Call() = 0;
	};

	typedef void(*state_change_callback)();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename TTrigger, typename TState>
	class IStateMap
	{
	public:

		virtual ~IStateMap() { /* Needs to remain empty */ }

		virtual bool Add(const TState& state, IState<TTrigger, TState>* instance) = 0;

		virtual IState<TTrigger, TState>* Get(const TState& state) const = 0;

		virtual bool Has(const TState& state) const = 0;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define FSM_STATE(TRIGGER, STATE) \
public IState<TRIGGER, STATE>, \
public IStateConfigurator<TRIGGER, STATE>

}

#pragma endregion

/****************************************************************************************************************************/

#pragma region AUTO STATES

namespace FSM
{
	namespace ___IMPL___
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		CAutoState<TTrigger, TState>::CAutoState(const TState& state)
			: IState<TTrigger, TState>(state, true),
			m_pOnEntryCallback(nullptr), m_pOnExitCallback(nullptr)
		{
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		CAutoState<TTrigger, TState>::~CAutoState()
		{
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		void CAutoState<TTrigger, TState>::OnEntry()
		{
			Call(m_pOnEntryCallback);
			Call(m_pOnEntryCallbackInstance);
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		void CAutoState<TTrigger, TState>::OnExit()
		{
			Call(m_pOnExitCallback);
			Call(m_pOnExitCallbackInstance);
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::OnEntry(ICallback* onEntryCallback)
		{
			// Throw if already subscribed, to avoid memory leaks
			if (m_pOnEntryCallbackInstance != nullptr) throw new std::exception("Already subscribed to instance!");
			m_pOnEntryCallbackInstance = onEntryCallback;
			return this;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::OnEntry(state_change_callback onEntryCallback)
		{
			// Throw if already subscribed to instance
			if (m_pOnEntryCallbackInstance != nullptr) throw new std::exception("Already subscribed to instance!");
			m_pOnEntryCallback = onEntryCallback;
			return this;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		IStateConfigurator<TTrigger, TState>* CAutoState<TTrigger, TState>::OnExit(ICallback* onExitCallback)
		{
			// Throw if already subscribed, to avoid memory leaks
			if (m_pOnExitCallbackInstance != nullptr) throw new std::exception("Already subscribed to instance!");
			m_pOnExitCallbackInstance = onExitCallback;
			return this;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

/****************************************************************************************************************************/

#pragma region STATE MAP

namespace FSM
{
	namespace __IMPL__
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		class CStateMap : public IStateMap<TTrigger, TState>
		{
		private:
			std::map<TState, IState<TTrigger, TState>*> m_stateMap;

		public:

			CStateMap();
			virtual ~CStateMap();

			virtual bool Add(const TState& state, IState<TTrigger, TState>* instance) override;

			virtual IState<TTrigger, TState>* Get(const TState& state) const override;

			virtual bool Has(const TState& state) const override;
		};

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		inline CStateMap<TTrigger, TState>::CStateMap()
		{
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		inline CStateMap<TTrigger, TState>::~CStateMap()
		{
			typename std::map<TState, IState<TTrigger, TState>*>::iterator itr = m_stateMap.begin();
			while (!m_stateMap.empty())
			{
				typename std::map<TState, IState<TTrigger, TState>*>::iterator cloneItr = itr;
				++itr;

				IState<TTrigger, TState>* toDelete = cloneItr->second;
				m_stateMap.erase(cloneItr);

				if (toDelete != nullptr && toDelete->Disposable)
				{
					delete toDelete;
					toDelete = nullptr;
				}
			}
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		inline bool CStateMap<TTrigger, TState>::Add(const TState & state, IState<TTrigger, TState>* instance)
		{
			if (Has(state)) return false;

			m_stateMap.insert(std::make_pair(state, instance));

			return true;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		inline IState<TTrigger, TState>* CStateMap<TTrigger, TState>::Get(const TState & state) const
		{
			typename std::map<TState, IState<TTrigger, TState>*>::const_iterator itr = m_stateMap.find(state);
			return itr->second;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		template<typename TTrigger, typename TState>
		inline bool CStateMap<TTrigger, TState>::Has(const TState & state) const
		{
			typename std::map<TState, IState<TTrigger, TState>*>::const_iterator itr = m_stateMap.find(state);

			return itr != m_stateMap.end();
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
}


#pragma endregion

/****************************************************************************************************************************/

#pragma region STATE MACHINE

namespace FSM
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename TTrigger, typename TState>
	CFiniteStateMachine<TTrigger, TState>::CFiniteStateMachine(const TState& defaultState)
	{
		m_pMap = new __IMPL__::CStateMap<TTrigger, TState>();
		m_pCurrentState = this->Configure(defaultState)->State();
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename TTrigger, typename TState>
	inline CFiniteStateMachine<TTrigger, TState>::~CFiniteStateMachine()
	{
		if (m_pMap != nullptr)
		{
			delete m_pMap;
			m_pMap = nullptr;
		}
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename TTrigger, typename TState>
	inline const TState* CFiniteStateMachine<TTrigger, TState>::CurrentState() const
	{
		if (m_pCurrentState == nullptr) throw std::exception("Current state is null! This should not happen!");

		return &m_pCurrentState->StateType;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template<typename TTrigger, typename TState>
	inline bool CFiniteStateMachine<TTrigger, TState>::AddState(const TState & state, IState<TTrigger, TState>* instance)
	{
		if (m_pMap->Has(state)) return false;

		m_pMap->Add(state, instance);
		return true;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}


#pragma endregion

/****************************************************************************************************************************/