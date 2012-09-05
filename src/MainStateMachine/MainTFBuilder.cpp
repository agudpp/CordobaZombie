/*
 * MainTFBuilder.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "MainTFBuilder.h"

// States
#include "LoadingState.h"
#include "SponsorsState.h"


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
	LoadingState *loadingState = new LoadingState;
	SponsorsState *sponsors = new SponsorsState;

	loadingState->setLoaderManager(mLoaderManager);
//	tt->setStartState(loadingState);

	tt->setStartState(sponsors);

	return tt;
}
