/*
 * SelectableObject.cpp
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#include "SelectableObject.h"

#include "SelectionManager.h"


namespace selection {

/**
 * Add an object to the SelectionManager. This will automatically call
 * the functions objectSelected / objectUnselected
 */
void
SelectableObject::selectThisOne(void)
{
    SelectionManager::getInstance().select(this);
}

void
SelectableObject::unselectThisOne(void)
{
    SelectionManager::getInstance().unselect(this);
}

void
SelectableObject::configureEntity(Ogre::MovableObject *ent, unsigned int queryFlags,
        unsigned int visibilityFlags)
{
    ent->setQueryFlags(queryFlags);
    ent->setVisibilityFlags(visibilityFlags);
    ent->setUserAny(Ogre::Any(this));
}

}
