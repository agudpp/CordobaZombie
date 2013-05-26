/*
 * Test.cpp
 *
 *  Created on: 15/08/2011
 *      Author: agustin
 */

#include <OgreSceneNode.h>
#include <iostream>
#include <assert.h>
#include <OgreSceneQuery.h>
#include <OgreRay.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreString.h>

#include "GlobalObjects.h"
#include "Test.h"
#include "BackpackItem.h"
#include "WeaponBackpackItem.h"
#include "Weapon.h"
#include "Gun9mm.h"
#include "BackpackItemBuilder.h"

#include "BackpackDefines.h"


static double gettimestamp(void)
{
	struct timeval now;

	if(gettimeofday(&now, 0) < 0)
		return 0.0;

	double nowDouble = static_cast<double>(now.tv_sec + (
						static_cast<double>(now.tv_usec)/1000000.0));

	return nowDouble;
}

bool Test::performMouseRay(Ogre::Vector3 &v)
{
	// set up the ray
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mMouseCursor.getXRelativePos(),
			mMouseCursor.getYRelativePos());

	// check if the ray intersects our plane
	// intersects() will return whether it intersects or not (the bool value) and
	// what distance (the Real value) along the ray the intersection is
	std::pair<bool, Ogre::Real> result = mouseRay.intersects(mGlobalPlane);

	if (result.first)
	{
		// if the ray intersect the plane, we have a distance value
		// telling us how far from the ray origin the intersection occurred.
		// the last thing we need is the point of the intersection.
		// Ray provides us getPoint() function which returns a point
		// along the ray, supplying it with a distance value.

		// get the point where the intersection is
		v = mouseRay.getPoint(result.second);
		return true;
	}

	return false;
}

void Test::createBackpackItems(void)
{
	BackpackItemBuilder builder;
	builder.setFilename("BackpackItems.xml");

	BackpackItem *item = builder.createBackpackItem("9mm");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("Fal");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("BR");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("BT");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("Grenade");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("Machete");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("Mazo");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("Mina");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
	item = builder.createBackpackItem("Shotgun");
	ASSERT(item);
	mBackpack.addBackpackItem(item);
}

Test::Test()
{
	mMouseCursor.setImage("cursor.png");
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());

}

Test::~Test()
{

}

// handle input
void Test::handleInput(void)
{
	sm::Vector2 tran;
	static const float VEL = 150.0f;
	tran.x = tran.y = 0.0f;
	static bool keyPres1 = false;
	static bool keyPres = false;

	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

	static bool mousePressed = false;
	// Mouse input check
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Left)){
		if (!mousePressed)
		{
			mousePressed = true;
		}
	} else {
		mousePressed = false;
	}

}


/* Load additional info */
void Test::loadAditionalData(void)
{
	mMenuManager.build(GLOBAL_WINDOW->getWidth(),GLOBAL_WINDOW->getHeight(),
			5,5);
	IMenu::setMenuManager(&mMenuManager);

	ShootContainer *sc = new ShootContainer;
	Weapon::setShootContainer(sc);
	Gun9mm *weapon = new Gun9mm;
	weapon->setEntity(GLOBAL_SCN_MNGR->createEntity("9mm.mesh"));
	Weapon::Ammunition ammo;
	ammo.ammo = 123;
	weapon->setAmmunition(ammo);

	Ogre::OverlayManager &om =Ogre::OverlayManager::getSingleton();
	Ogre::Overlay *ov = om.getByName("BackpackItemsOverlay");
	ASSERT(ov);
	ov->show();
/*
	WeaponBackpackItem *item1 = new WeaponBackpackItem;
	item1->setWeapon(weapon);
	item1->setSection(BackpackDef::SECTION_WEAPON);
	item1->loadFromOverlay("BpItem/9mm");
	item1->show();

	WeaponBackpackItem *item2 = new WeaponBackpackItem;
	item2->setWeapon(weapon);
	item2->setSection(BackpackDef::SECTION_WEAPON);
	item2->configureAll(static_cast<Ogre::OverlayContainer *>(
			om.getOverlayElement("BpItem/Fall")));
	item2->setVisible(true);
	item2->setEnable(true);
	mBackpack.addBackpackItem(item1);
	mBackpack.addBackpackItem(item2);*/

	createBackpackItems();

}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();

	static double t1,t2,t3,t4;
	static bool keyPres = false;
	static bool keyPres1 = false;
	static bool keyPres2 = false;
	sm::Vector2 p;

	t1 = gettimestamp();
	t3 = t2 - t1;
	// update the game objects
	t2 = gettimestamp();

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_P)) {
		if (!keyPres) {
			keyPres = true;
			std::cout << "Printing Times:\n" << "LastFrameTime: "
					<< GLOBAL_TIME_FRAME << "\n" << "Time of sim: " << t2 - t1
					<< "\n" << "Time of rendering and more: " << t3
					<< std::endl;

			std::cout << mBodySceneNode->getPosition().x << "\t" <<
					mBodySceneNode->getPosition().z << std::endl;
		}
	} else {
		keyPres = false;
	}

	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_1)) {
		if(!keyPres1){
			static bool hided = false;
			keyPres1 = true;
			if(hided)	{mBackpack.show(); hided = false;}
			else		{mBackpack.hide(); hided = true;}
		}
	} else {
		keyPres1 = false;
	}
	if (GLOBAL_KEYBOARD->isKeyDown(OIS::KC_2)) {
		if(!keyPres2){
			static bool hided = false;
			keyPres2 = true;
			if(hided)	{mBackpack.showBackpack(); hided = false;}
			else		{mBackpack.hideBackpack(); hided = true;}
		}
	} else {
		keyPres2 = false;
	}

	mMenuManager.update();

}
