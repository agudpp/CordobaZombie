/*
 * WeaponBE.cpp
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#include "WeaponBE.h"

#include <boost/bind.hpp>

#include <GameUnits/PlayerUnit/Backpack/BackpackDefines.h>
#include <GameUnits/PlayerUnit/PlayerUnit.h>
#include <GameUnits/Weapon/Weapon.h>

namespace hud {

WeaponBE::WeaponBE(BackpackItemPtr& item, CallbackMenuButtonPtr& button) :
    BackpackElement(item, button)
{
    ASSERT(item.get());
    ASSERT(item->type == BackpackDef::ItemType::WEAPON);

    // we will connect the signal to the weapon
    Weapon* weapon = 0;
    item->getUserDef(weapon);
    mConnection = weapon->addCallback(boost::bind(&WeaponBE::weaponChanged, this, _1));
}

WeaponBE::~WeaponBE()
{

}

void
WeaponBE::buttonPressed(CallbackMenuButton::ButtonID button)
{
    if (button != CallbackMenuButton::ButtonID::LEFT_BUTTON) {
        // do nothing
        debugWARNING("We should show the information of the weapon here probably\n");
        return;
    }

    // we just press the left mouse button in the weapon, set the current weapon
    // to the player
    ASSERT(mPlayer);
    Weapon* weapon = 0;
    getBackpackItemPtr()->getUserDef(weapon);
    ASSERT(weapon);

    // if the actual weapon of the player is this weapon, we do nothing
    if (mPlayer->getActualWeapon() == weapon) {
        // the same weapon, do nothing
        return;
    }

    mPlayer->setWeapon(weapon);
}

void
WeaponBE::update(void)
{
    ASSERT(false); // TODO implement this
}


void
WeaponBE::weaponChanged(Weapon*)
{
    if (!isVisible()) {
        // we don't want to update anything since we are not visible,
        // we will mark this as dirty and hope that when we have to show us
        // we will update the info
        mIsDirty = true;
        return;
    }
    // Update the element data here
    debugERROR("TODO: we need to actualize here the data of the weapon, for example"
        " how many ammunition the weapon has\n");
}

} /* namespace hud */
