/*
 * MainTFBuilder.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */


#include "MainTFBuilder.h"

#include "DebugUtil.h"
#include "MainStateMachineDefs.h"
// States
#include "FirstInformationState.h"
#include "SponsorsState.h"
#include "MainMenuState.h"

#include "LoadingState.h"
#include "ExitState.h"

#include "VideoState.h"


MainTFBuilder::MainTFBuilder()
{
}

MainTFBuilder::~MainTFBuilder()
{
}


/**
 * Build Transition function from xml?
 */
MainTransitionFunction *MainTFBuilder::build(const TiXmlElement  *elem)
{
	// TODO
	debugRED("TODO\n");

	// FIXME ahora hacemos hardcoded
	MainTransitionFunction *tt = new MainTransitionFunction;

	//states
	FirstInformationState *firstInformation= new FirstInformationState;
	SponsorsState *sponsors = new SponsorsState;
	VideoState *video = new VideoState;
	MainMenuState *mainMenu = new MainMenuState;
	LoadingState *loadingState = new LoadingState;
	ExitState *exitState = new ExitState;



	loadingState->setLoaderManager(mLoaderManager);
	// TODO: borrar estas dos líneas para que luego siga cargando todo
	//tt->setStartState(loadingState);
	//return tt;

	// build the transitions
	tt->setStartState(firstInformation);

	tt->addNewEntry(firstInformation, MainMachineEvent::MME_DONE, video);
	tt->addNewEntry(video, MainMachineEvent::MME_DONE, sponsors);
	tt->addNewEntry(sponsors, MainMachineEvent::MME_DONE, mainMenu);
	tt->addNewEntry(mainMenu, MainMachineEvent::MME_DONE, loadingState);
	tt->addNewEntry(mainMenu, MainMachineEvent::MME_EXIT, exitState);

	// TODO: tenemos que agregar newGame -> loading state por ejemplo?


	return tt;
}
