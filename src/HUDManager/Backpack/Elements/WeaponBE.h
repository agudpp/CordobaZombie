/*
 * WeaponBE.h
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#ifndef WEAPONBE_H_
#define WEAPONBE_H_


#include <GameUnits/Weapon/Weapon.h>
#include <GameUnits/PlayerUnit/Backpack/BackpackItem.h>
#include <MenuSystem/GUI/Button/MenuButton.h>
#include <Common/DebugUtil/DebugUtil.h>

#include "../BackpackElement.h"

namespace hud {

class WeaponBE : public BackpackElement
{
public:
    WeaponBE(BackpackItemPtr& item, CallbackMenuButtonPtr& button);
    virtual
    ~WeaponBE();

    /**
     * @brief Callback for when the button is pressed
     */
    virtual void
    buttonPressed(CallbackMenuButton::ButtonID) = 0;

    /**
     * @brief Virtual method that will be called everytime we need to update
     *        the element (when is dirty for example).
     *        In this method we should put all the logic for update the visual
     *        information
     */
    virtual void
    update(void);

    /**
     * @brief We need to handle the weapon change events here, this will handle
     *        that
     */
    void
    weaponChanged(Weapon*);


private:
    Weapon::Connection mConnection;
};

} /* namespace hud */
#endif /* WEAPONBE_H_ */
