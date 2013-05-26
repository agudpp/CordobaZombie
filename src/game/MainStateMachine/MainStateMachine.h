/*
 * MainStateMachine.h
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#ifndef MAINSTATEMACHINE_H_
#define MAINSTATEMACHINE_H_

#include "IMainState.h"
#include "MainTransitionFunction.h"
#include "MainStateMachineDefs.h"

class MainStateMachine
{
public:
	MainStateMachine();
	~MainStateMachine();

	/**
	 * Set the Transition function.
	 * This class DO NOT free any memory
	 */
	void setTransitionFunction(MainTransitionFunction *mtf);

	/**
	 * Start the machine
	 */
	void start(void);

	/**
	 * Stop the machine / reset
	 */
	void stop(void);
	void reset(void);

	/**
	 * Get the MainMachineInfo to configure it
	 */
	MainMachineInfo &getInfo(void);

private:
	// Functions used to load/unload all the resources needed/used by the
	// actual state
	void loadResources(IMainState *);
	void unloadResources(IMainState *);

private:
	MainTransitionFunction 		*mTransitionFunction;
	MainMachineEvent			mLastEvent;
	MainMachineInfo				mInfo;
	IMainState					*mActualState;
	IMainState					*mLastState;
	bool						mRunning;
};




#endif /* MAINSTATEMACHINE_H_ */
