/*
 * EmptySelection.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: agustin
 */

#include "EmptySelection.h"

#include <OgreSceneQuery.h>

#include <debug/DebugUtil.h>
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
    // we only need to raycast and get the first object...
       selection::SelectableObject *selObj = getFirstRaycastedObj(RAYCAST_MASK);
       newRaycastedObject(selObj);
}


} /* namespace input */
