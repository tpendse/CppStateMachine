#pragma once

#include "FSMInterfaces.h"
#include <map>

#pragma region STATE MAP

namespace FSM
{
	namespace __IMPL__
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		inline CStateMap<TTrigger, TState>::CStateMap()
		{
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		inline bool CStateMap<TTrigger, TState>::Add(const TState & state, IState<TTrigger, TState>* instance)
		{
			if (Has(state)) return false;

			m_stateMap.insert(std::make_pair(state, instance));

			return true;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		inline IState<TTrigger, TState>* CStateMap<TTrigger, TState>::Get(const TState & state) const
		{
			typename std::map<TState, IState<TTrigger, TState>*>::const_iterator itr = m_stateMap.find(state);
			return itr->second;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename TTrigger, typename TState>
		inline bool CStateMap<TTrigger, TState>::Has(const TState & state) const
		{
			typename std::map<TState, IState<TTrigger, TState>*>::const_iterator itr = m_stateMap.find(state);

			return itr != m_stateMap.end();
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}


#pragma endregion