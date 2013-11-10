/*
 * HUDbullet.h
 *
 *  Created on: Nov 10, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#ifndef HUDBULLET_H_
#define HUDBULLET_H_

#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>

#include <main_player/weapon/WeaponDefs.h>
#include <main_player/weapon/Weapon.h>
#include "HUDelement.h"

namespace cz {

class HUDbullet : public HUDelement
{
public:
	HUDbullet();
	virtual ~HUDbullet();

	/**
	 * @brief Set element overlay container visibility to 'visible'
	 */
	void
	setVisible(bool visible);

	/**
	 * @brief  Fill in the structure (create overlays, parse files, etc)
	 * @return true on success, false otherwise
	 */
	bool
	build(Ogre::Overlay* ov);

	/**
	 * @brief Set current bullet type according to 'id'
	 */
	void
	setBulletType(WeaponID id);

	/**
	 * @brief Set magazine bullets count and bullets left, according to 'w'
	 */
	void
	setBulletCount(Weapon* w);

private:
	Ogre::TextAreaOverlayElement* mBulletCount;	// Bullet count display
	Ogre::PanelOverlayElement* mBulletPanel;	// Bullet type  display
};


///////////////////////////////////////////////////////////////////////////////
inline void
HUDbullet::setVisible(bool visible)
{
	if (!mBulletCount) {
		debugERROR("Called before creating the bullets count text panel.\n");

	} else if (!mBulletPanel) {
		debugERROR("Called before creating the bullets type panel.\n");

	} else if (visible && !mBulletPanel->isVisible()) {
		mBulletCount->show();
		mBulletPanel->show();

	} else if (!visible && mBulletPanel->isVisible()) {
		mBulletCount->hide();
		mBulletPanel->hide();
	}
}


///////////////////////////////////////////////////////////////////////////////
inline void
HUDbullet::setBulletType(WeaponID id)
{
	if (!mBulletCount) {
		debugERROR("Called before creating the bullets count text panel.\n");

	} else if (!mBulletPanel) {
		debugERROR("Called before creating the bullets type panel.\n");

	} else if (id >= WID_COUNT) {
		debugERROR("Undefined WeaponID: %d\n", id);

	} else {
		// Set texture UV coordinates for selected weapon
		const ui::UVCoord* uv = &WeaponUV[id];
		mBulletPanel->setUV(uv->u0, uv->v0, uv->u1, uv->v1);
	}
}



} /* namespace cz */

#endif /* HUDBULLET_H_ */
