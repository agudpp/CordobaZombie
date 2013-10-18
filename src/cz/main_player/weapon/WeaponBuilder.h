/*
 * WeaponBuilder.h
 *
 *  Created on: Oct 10, 2013
 *      Author: agustin
 */

#ifndef WEAPONBUILDER_H_
#define WEAPONBUILDER_H_

namespace cz {

// Forward
//
class Weapon;

namespace WeaponBuilder {

// @brief Construct a weapon given an ID. It will return a new allocated instance
//        of the weapon and with the entity and scene node already set.
// @param wid       The weapon id.
// @return a new allocated (so you need to free the memory later) of the weapon
//         to be used. Or 0 on error
//
Weapon*
buildWeapon(WeaponID wid);

};

} /* namespace cz */
#endif /* WEAPONBUILDER_H_ */
