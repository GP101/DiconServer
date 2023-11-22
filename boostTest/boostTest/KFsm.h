/// @file   KFsm.h
/// @date   jintaeks on 2017-08-24_01-12
#pragma once

#include <stdarg.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include "KGen.h"
#include "EnumToString.h"


class KFsmState;
typedef std::shared_ptr<KFsmState>    KFsmStatePtr;

class KFsmState
{
public:
    const static int    INVALID_STATE = -1;

                        KFsmState(int iStateId_)
                            : m_iStateId( iStateId_ )
                        {
                            m_vecTransition.clear();
                        }//KFsmState()

    int                 GetStateId() const { return m_iStateId;  }

    int                 GetNumTransition() const 
                        {
                            return m_vecTransition.size();
                        }//GetNumTransition()

	void                AddTransition(int iInput_, int iDestState_)
						{
							m_vecTransition.push_back(std::make_pair(iInput_, iDestState_));
						}//AddTransition()

	void                AddTransition(int iInput_, const KFsmState& destState_)
						{
							m_vecTransition.push_back(std::make_pair(iInput_, destState_.GetStateId()));
						}//AddTransition()

						// return stateId for input 'iInput_'
    int                 GetDestState(int iInput_) const
                        {
                            std::vector<std::pair<int, int>>::const_iterator itor
                                = std::find_if(m_vecTransition.begin(), m_vecTransition.end(), [=](const std::pair<int, int>& t){ return t.first == iInput_; });
                            if (itor != m_vecTransition.end() ) {
                                return itor->second;
                            }//if
                            return INVALID_STATE;
                        }//GetDestState()

						/// @param	iNumStates_ : number of states listed
						/// @param	... : state list as variable arguments
    bool                CheckState(int iNumStates_, ...)
                        {
                            va_list ap;
                            va_start(ap, iNumStates_);

                            for (int i = 0; i < iNumStates_; ++i ) {
                                if (va_arg(ap, int) == m_iStateId) return true;
                            }//for

                            va_end(ap);
                            return false;
                        }//CheckState()

private:
    int                 m_iStateId;
    std::vector<std::pair<int, int>>
                        m_vecTransition; // first: input, second: destination state
};//class KFsmState


class KFsm;
typedef std::shared_ptr<KFsm>     KFsmPtr;

class KFsm
{
    std::map<int, KFsmStatePtr>
                        m_mapFsmStates; // first: stateId, second: KFsmState information
    int                 m_iCurrentStateId;

public:
    CONSTRUCTOR         KFsm( int iStateID )
                        {
                            m_iCurrentStateId = iStateID;
                        }
    DESTRUCTOR          ~KFsm(){}

    int                 GetCurrentStateId() const { return m_iCurrentStateId; }
    void                SetCurrentStateId( int iStateId_ ) { m_iCurrentStateId = iStateId_; }

    KFsmStatePtr        GetCurrentState() const
                        {
                            return GetState( m_iCurrentStateId );
                        }

    /// @brief  get state by Id.
    KFsmStatePtr        GetState(int iStateId_) const
                        {
                            auto mitor = m_mapFsmStates.find(iStateId_);
                            if (mitor != m_mapFsmStates.end())
                            {
                                return mitor->second;
                            }//if
                            return nullptr;
                        }
    bool                AddState(const KFsmState& fsmState)
                        {
                            KFsmStatePtr spState;
                            spState.reset(new KFsmState(0));
                            *spState = fsmState;
                            return AddState(spState);
                        }
    bool                AddState(KFsmStatePtr spFsmState)
    {
                            if (spFsmState == nullptr)
                                return false;

                            const int iStateId = spFsmState->GetStateId();
                            auto mitor = m_mapFsmStates.insert(std::make_pair(iStateId, spFsmState));
                            return mitor.second;
                        }//AddState()

    int                 DoTransition(int iInput_)
                        {
                            KFsmStatePtr spCurrentState = GetState(m_iCurrentStateId);
                            if (spCurrentState == nullptr)
                                return KFsmState::INVALID_STATE;

                            const int iCurrentStateId = spCurrentState->GetDestState(iInput_);
							if (iCurrentStateId != KFsmState::INVALID_STATE)
								m_iCurrentStateId = iCurrentStateId; // update current state if valid
                            return iCurrentStateId;
                        }//DoTransition()
};//class KFsm

/** @example class KFsm

            int main()
            {
            KFsmState       state1(0);
            KFsmState       state2(1);
            KFsm            stateMachine(0);

            state1.AddTransition(10, 1);
            state1.AddTransition(20, 2);
            stateMachine.AddState(state1);

            state2.AddTransition(30, 0);
            stateMachine.AddState(state2);

            KFsmStatePtr spCurrentState = stateMachine.GetCurrentState();
            printf("state %i\r\n", spCurrentState->GetStateId());
            stateMachine.DoTransition(10);
            spCurrentState = stateMachine.GetCurrentState();
            printf("state %i\r\n", spCurrentState->GetStateId());
            stateMachine.DoTransition(30);
            stateMachine.DoTransition(20);
            spCurrentState = stateMachine.GetCurrentState();

            if (spCurrentState != nullptr)
            {
            const bool isState = spCurrentState->CheckState(2, 0, 1);
            printf("isState %i\r\n", isState);
            }//if
            }//main()

            */

class KFsmBase
{
public:
    void                SetFsm( KFsmPtr& spFSM )
                        {
                            std::lock_guard<std::mutex> lock(m_muFsm);
                            m_spFsm = spFSM;
                        }
    KFsmPtr             GetFsm()
                        {
                            std::lock_guard<std::mutex> lock(m_muFsm);
                            return m_spFsm;
                        }
    int                 GetStateId() const
                        {
                            std::lock_guard<std::mutex> lock(m_muFsm);
                            return m_spFsm->GetCurrentState()->GetStateId();
                        }
    bool                StateTransition( int nInput )
                        {
                            std::lock_guard<std::mutex> lock(m_muFsm);
                            const int iState = m_spFsm->DoTransition(nInput);
                            return iState != KFsmState::INVALID_STATE;
                        }

protected:
    mutable std::mutex  m_muFsm;
    KFsmPtr             m_spFsm;
};

#define DECLARE_FSM  \
    public: \
        void SetFsm( KFsmPtr& spFSM ) \
        { \
            std::lock_guard<std::mutex> lock(m_muFsm); \
            m_spFsm = spFSM; \
        } \
        KFsmPtr GetFsm() \
        { \
            std::lock_guard<std::mutex> lock(m_muFsm); \
            return m_spFsm; \
        } \
        int GetStateId() \
        { \
            std::lock_guard<std::mutex> lock(m_muFsm); \
            return m_spFsm->GetCurrentState()->GetStateId(); \
        } \
        bool StateTransition( int nInput ) \
        { \
            std::lock_guard<std::mutex> lock(m_muFsm); \
            const int iState = m_spFsm->DoTransition( nInput ); \
            return iState != KFsmState::INVALID_STATE; \
        } \
protected: \
    mutable std::mutex  m_muFsm;   \
    KFsmPtr             m_spFsm


#define VERIFY_STATE( ... ) \
    if( !m_spFsm->GetCurrentState()->CheckState(PRN_ENUM_TEXT_NARG(__VA_ARGS__), __VA_ARGS__) ) \
    { \
        BEGIN_LOG( std::cerr, L"error" ) \
        << L"    Current State : " << GetStateId() << std::endl \
        << L"    Valid State : " L ## #__VA_ARGS__ << std::endl; \
        return; \
    }
