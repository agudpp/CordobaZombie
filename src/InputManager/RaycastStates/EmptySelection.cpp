/*
 * EmptySelection.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: agustin
 */

#include "EmptySelection.h"

#include <OgreSceneQuery.h>

#include <Common/DebugUtil/DebugUtil.h>
#include <SelectionSystem/SelectionData.h>
#include <OgreMovableObject.h>

namespace input {

EmptySelection::EmptySelection()
{
    // TODO Auto-generated constructor stub

}

EmptySelection::~EmptySelection()
{
    // TODO Auto-generated destructor stub
}

void
EmptySelection::configure(const selection::SelectionData &selData)
{
    // we should ensure that there are absolutly nothing selected
    ASSERT(selData.selected.empty());
}

void
EmptySelection::executeRayCast(void)
{
    // We execute the Ogre raycast instead of getting the position in the
    // plane where the player will be moving.
    // We only want the first object that is closer to the camera over the ray
    Ogre::RaySceneQueryResult &results = performRayQuery(RAYCAST_MASK);
    if (results.empty()){
        // no results found
        newRaycastedObject(0);
        return;
    }
    // else there are new raycasted objects, we want only the first one
    Ogre::RaySceneQueryResultEntry &obj = results.front();
    ASSERT(obj.movable && obj.worldFragment == 0);

    // check if we have something in the user any
    const Ogre::Any &anyObj = obj.movable->getUserAny();
    if (anyObj.isEmpty()) {
        newRaycastedObject(0);
        return;
    }

    // advise the object that we are over him with the mouse
    // Thanks to Ogre we have to do a any_cast that is a string comparison!!!!
    // so nice! so efficient
    selection::SelectableObject *selObj =
        Ogre::any_cast<selection::SelectableObject*>(anyObj);
    newRaycastedObject(selObj);
}


} /* namespace input */
