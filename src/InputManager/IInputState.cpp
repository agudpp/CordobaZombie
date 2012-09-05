/*
 * IInputState.cpp
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */
#include "IInputState.h"

InputManager		*IInputState::mInputManager = 0;
LevelManager		*IInputState::mLevelManager = 0;
CollisionResult		IInputState::mCollObjs;

////////////////////////////////////////////////////////////////////////////////
void IInputState::setInpuManager(InputManager *im)
{
	ASSERT(im);
	mInputManager = im;
}
void IInputState::setLevelManager(RaycastManager *im)
{
	ASSERT(im);
	mLevelManager = im;
}

////////////////////////////////////////////////////////////////////////////////
IInputState::IInputState()
{
	ASSERT(mInputManager);
	ASSERT(mLevelManager);
}
virtual ~IInputState()
{

}


////////////////////////////////////////////////////////////////////////////////
IInputState::UnitType IInputState::getUnitSelectedType(void)
{
	ASSERT(false);
	// TODO
}
