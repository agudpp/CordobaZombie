/*
 * StateMachine.h
 *
 *  Created on: 09/02/2012
 *      Author: agustin
 *
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_


#include "DebugUtil.h"
#include "SMDefines.h"
#include "SMTransitionTable.h"

namespace ia {

template<typename T>
class StateMachine {
public:
	StateMachine(T ref, const SMTransitionTable<T> *tt = 0):
		mActualState(0),
		mLastState(0),
		mLastEvent(0),
		mTTable(tt),
		mRef(ref)
	{

	}

	~StateMachine()
	{

	}

	/* Sets the SMTransition table */
	void setTransitionTable(const SMTransitionTable<T> *tt)
	{
		if(mTTable){
			// we already have a table
			ASSERT(false);
		}
		mTTable = tt;
	}

	/* Start the FSM */
	void start(void)
	{
		ASSERT(mTTable);
		ASSERT(mTTable->getStartState());
		ASSERT(mActualState == 0);

		mActualState = mTTable->getStartState();
		mActualState->enter(mRef);
	}

	/* Reset the FSM */
	void reset(void)
	{
		ASSERT(mTTable);
		ASSERT(mTTable->getStartState());

		mLastEvent = 0;
		changeState(mTTable->getStartState());
	}


	/* Go back to the last State */
	inline void executeLastState(void)
	{
		changeState(mLastState);
	}

	/* Returns the actual state and the last state*/
	inline IState<T> *getActualState(void) const {return mActualState;}
	inline IState<T> *getLastState(void) const {return mLastState;}

	/* Get the last event executed */
	inline SMEvent getLastEvent(void) const {return mLastEvent;}

	/* Force to change the state. BE CAREFULL WITH THIS FUNCTION (we are avoiding
	 * the table of this StateMachine
	 */
	void forceChangeState(IState<T> *s)
	{
		ASSERT(s);
		changeState(s);
	}

	/* Receive an external input */
	inline void newEvent(SMEvent e)
	{
		ASSERT(mTTable);
		ASSERT(mActualState);
		//std::tr1::HashTable
		// get the next state
		IState<T> *nextState = mTTable->getNextState(mActualState, e);
		if(!nextState){
			debug("We cannot get next state from actual state: %d"
					" and event %d\n", mActualState->getID(), e);
//			return;
		} else {
			// stop the actual event
			mLastEvent = e;
			changeState(nextState);
		}
	}

	/* Update the state machine */
	inline void update(void)
	{
		ASSERT(mActualState);
		mActualState->update(mRef);
	}


private:
	// change states
	inline void changeState(IState<T> *newS)
	{
		mActualState->exit(mRef);
		mLastState = mActualState;
		mActualState = newS;
		mActualState->enter(mRef);
	}

private:
	IState<T>					*mActualState;
	IState<T>					*mLastState;
	SMEvent						mLastEvent;
	const SMTransitionTable<T>	*mTTable;
	T							mRef;

};

}

#endif /* STATEMACHINE_H_ */
