/*
 * SinglePlayerSel.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: agustin
 */

#include "SinglePlayerSel.h"

#include <OgreSceneQuery.h>

#include <DebugUtil/DebugUtil.h>
#include <SelectionSystem/SelectionData.h>
#include <SelectionSystem/SelectionType.h>
#include <SelectionSystem/SelectionManager.h>

namespace input {

SinglePlayerSel::SinglePlayerSel()
{
    // TODO Auto-generated constructor stub

}

SinglePlayerSel::~SinglePlayerSel()
{
    // TODO Auto-generated destructor stub
}

void
SinglePlayerSel::configure(const selection::SelectionData &selData)
{
    ASSERT(!selData.selected.empty());

    // fill the players vec with the selected players
    mPlayers.clear();
    sSelectionMngr->getObjects(selection::Type::SEL_TYPE_PLAYER, mPlayers);
    ASSERT(mPlayers.size() == 1); // ensure only one player is selected
}

void
SinglePlayerSel::executeRayCast(void)
{
    // we only need to raycast and get the first object...
    selection::SelectableObject *selObj = getFirstRaycastedObj(RAYCAST_MASK);
    newRaycastedObject(selObj);
}



} /* namespace input */
