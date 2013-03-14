/*
 * IInputState.cpp
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */
#include "IInputState.h"

namespace input {

selection::SelectionManager *IInputState::sSelectionMngr = 0;
LevelManager *IInputState::sLevelManager = 0;
CollisionResult IInputState::mCollObjs;
selection::SelectableObject *IInputState::sOnMouseOverObj = 0;

////////////////////////////////////////////////////////////////////////////////
void
IInputState::setLevelManager(LevelManager *lm)
{
	ASSERT(lm);
	sLevelManager = lm;
}

void
IInputState::setSelectionManager(selection::SelectionManager *sm)
{
	ASSERT(sm);
	sSelectionMngr = sm;
}

////////////////////////////////////////////////////////////////////////////////
IInputState::IInputState()
{
	ASSERT(sLevelManager);
	ASSERT(sSelectionMngr);
}

IInputState::~IInputState()
{

}


////////////////////////////////////////////////////////////////////////////////
IInputState::UnitType
IInputState::getUnitSelectedType(void)
{
	ASSERT(false);
	// TODO
}

}
