/*
 * InputStateMachine.h
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#ifndef INPUTSTATEMACHINE_H_
#define INPUTSTATEMACHINE_H_

#include "DebugUtil.h"
#include "InputTransitionTable.h"
#include "InputActionObject.h"
#include "IInputState.h"

class InputStateMachine {
public:
	inline InputStateMachine();
	inline ~InputStateMachine();

	/**
	 * Set the transition table to be used
	 * The memory must be freed by the creator class, this class will NOT remove
	 * any allocated memory
	 */
	inline void setTransitionTable(const InputTransitionTable *tt);

	/**
	 * Start the FSM
	 */
	inline void start(void);

	/**
	 * Reset the FSM
	 */
	inline void reset(void);


	/**
	 * Go back to the last State
	 */
	inline void executeLastState(void);

	/**
	 * Returns the actual state and the last state
	 */
	inline IInputState *getActualState(void) const;
	inline IInputState *getLastState(void) const;

	/**
	 * Get the last event executed
	 */
	inline int getLastEvent(void) const;

	/**
	 * Force to change the state. BE CAREFULL WITH THIS FUNCTION (we are avoiding
	 * the table of this StateMachine
	 */
	inline void forceChangeState(IInputState *s);

	/**
	 * Receive an external input
	 * @param e		The event
	 * @param ao	The action object to pass (if any)
	 */
	inline void newEvent(int e, InputActionObject *ao = 0);

	/**
	 * Update the state machine
	 */
	inline void update(void);


private:
	// change states
	inline void changeState(IInputState *newS, InputActionObject *ao = 0);

private:
	IInputState					*mActualState;
	IInputState					*mLastState;
	int							mLastEvent;
	const InputTransitionTable	*mTT;

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline InputStateMachine::InputStateMachine() :
		mTT(0),
		mActualState(0),
		mLastState(0),
		mLastEvent(0)
{

}
////////////////////////////////////////////////////////////////////////////////
inline InputStateMachine::~InputStateMachine()
{

}

////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::setTransitionTable(const InputTransitionTable *tt)
{
	ASSERT(tt);
	mTT = tt;
}

////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::start(void)
{
	ASSERT(mTT);
	ASSERT(mTT->getStartState());
	ASSERT(mActualState == 0);

	mActualState = mTTable->getStartState();
	mActualState->enter();
}

////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::reset(void)
{
	ASSERT(mTT);
	ASSERT(mTT->getStartState());

	mLastEvent = 0;
	changeState(mTTable->getStartState());
}

////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::executeLastState(void)
{
	changeState(mLastState);
}

////////////////////////////////////////////////////////////////////////////////
inline IInputState *InputStateMachine::getActualState(void) const
{
	return mActualState;
}
inline IInputState *InputStateMachine::getLastState(void) const
{
	return mLastState;
}

////////////////////////////////////////////////////////////////////////////////
inline int InputStateMachine::getLastEvent(void) const
{
	return mLastEvent;
}

////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::forceChangeState(IInputState *s)
{
	ASSERT(s);
	changeState(s);
}

////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::newEvent(int e, InputActionObject *ao)
{
	ASSERT(mTT);
	ASSERT(mActualState);
	IInputState *nextState = mTTable->getNextState(mActualState, e);
	if(!nextState){
		debugWARNING("We cannot get next state from actual state: %d"
				" and event %d\n", static_cast<int>(mActualState), e);
	} else {
		// stop the actual event
		mLastEvent = e;
		changeState(nextState, ao);
	}
}

////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::update(void)
{
	ASSERT(mActualState);
	mActualState->update();
}


////////////////////////////////////////////////////////////////////////////////
inline void InputStateMachine::changeState(IInputState *newS,
		InputActionObject *ao)
{
	mActualState->exit();
	mLastState = mActualState;
	mActualState = newS;
	mActualState->enter(ao);
}



#endif /* INPUTSTATEMACHINE_H_ */
