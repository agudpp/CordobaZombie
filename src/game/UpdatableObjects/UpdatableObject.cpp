/*
 * UpdatableObject.cpp
 *
 *  Created on: 18/04/2012
 *      Author: agustin
 *
 */

#include "DebugUtil.h"
#include "UpdatableObject.h"

UpdObjsManager	*UpdatableObject::mMngr = 0;


/**
 * Set the UpdatableObjectManager
 */
void UpdatableObject::setUpdObjsManager(UpdObjsManager *mngr)
{
	ASSERT(mngr);
	mMngr = mngr;
}
