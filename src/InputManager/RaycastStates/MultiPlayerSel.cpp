/*
 * MultiPlayerSel.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: agustin
 */

#include "MultiPlayerSel.h"

namespace input {

MultiPlayerSel::MultiPlayerSel()
{
    mPlayers.reserve(3);

}

MultiPlayerSel::~MultiPlayerSel()
{
}


void
MultiPlayerSel::configure(const selection::SelectionData &selData)
{
    ASSERT(!selData.selected.empty());

    // fill the players vec with the selected players
    mPlayers.clear();
    sSelectionMngr->getObjects(selection::Type::SEL_TYPE_PLAYER, mPlayers);
    ASSERT(mPlayers.size() > 1); // ensure more than one player is selected
}

void
MultiPlayerSel::executeRayCast(void)
{
    // we only need to raycast and get the first object...
    selection::SelectableObject *selObj = getFirstRaycastedObj(RAYCAST_MASK);
    newRaycastedObject(selObj);
}

} /* namespace input */
