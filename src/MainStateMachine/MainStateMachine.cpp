/*
 * MainStateMachine.cpp
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "MainStateMachine.h"

MainStateMachine::MainStateMachine() :
mActualState(0),
mLastState(0),
mTransitionFunction(0)
{

}

MainStateMachine::~MainStateMachine()
{

}



/**
 * Set the Transition function.
 * This class DO NOT free any memory
 */
void MainStateMachine::setTransitionFunction(MainTransitionFunction *mtf)
{
	ASSERT(mtf);
	ASSERT(!mTransitionFunction);

	mTransitionFunction = mtf;

}

/**
 * Start the machine
 */
void MainStateMachine::start(void)
{
	ASSERT(mTransitionFunction);
	ASSERT(mTransitionFunction->getStartState());
	ASSERT(mActualState == 0);

	mActualState = mTransitionFunction->getStartState();
	mRunning = true;

	debugColor(DEBUG_BROWN, "Starting MACHINE!\n");

	while(mRunning){
		//starting state
		debugColor(DEBUG_BROWN, "Entering new state: %s\n",
				mActualState->getName().c_str());

		mActualState->enter(mInfo);

		// updating state
		debugColor(DEBUG_BROWN, "Updating state: %s\n",
				mActualState->getName().c_str());

		mLastEvent = mActualState->update(mInfo);
		IMainState *newState = mTransitionFunction->getNextState(mActualState,
				mLastEvent);

		// changing state
		debugColor(DEBUG_BROWN, "Exiting old state: %s\nNew event %d received\n",
				mActualState->getName().c_str(), mLastEvent);
		ASSERT(newState);
		mLastState = mActualState;
		mActualState = newState;
		mLastState->exit();

	}

}

/**
 * Stop the machine / reset
 */
void MainStateMachine::stop(void)
{
	mRunning = false;
}
void MainStateMachine::reset(void)
{
	ASSERT(mTransitionFunction);
	ASSERT(mTransitionFunction->getStartState());

	mLastEvent = MME_DONE;
	mActualState = mLastState = 0;
}


MainMachineInfo &MainStateMachine::getInfo(void)
{
	return mInfo;
}
