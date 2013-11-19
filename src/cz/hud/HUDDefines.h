/*
 * HUDDefines.h
 *
 *  Created on: Nov 5, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#ifndef HUDDEFINES_H_
#define HUDDEFINES_H_


namespace cz {

/**
 * @remarks "XXX_NAME" must equal overlay and panel names in
 * 			Dropbox/cordobazombie0.1/game/hud/HUD.overlay
 */

// @brief Hud overlay name, for Ogre::OverlayManager handling
#define  HUD_OV_NAME  "Overlays/HUD"

// @brief Weapon HUDelement name
#define  HUDWEAPON_PANEL_NAME  "HUD/WeaponsPics"

// @brief Bullets HUDelement name (image)
#define  HUDBULLET_PANEL_NAME  "HUD/BulletsPics"

// @brief Bullets HUDelement name (text)
#define  HUDBULLET_TEXT_NAME   "HUD/BulletsCount"

}

#endif /* HUDDEFINES_H_ */