/*
 * IInputState.cpp
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */
#include "IInputState.h"

#include <SelectionSystem/SelectionManager.h>

namespace input {

selection::SelectionManager *IInputState::sSelectionMngr =
    &selection::SelectionManager::getInstance();
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
