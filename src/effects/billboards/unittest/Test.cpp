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

#include "GlobalObjects.h"
#include "Test.h"



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



// handle input
void Test::handleInput(void)
{
	///////////////////////////////////////////////////////////////////////////
	// CAMERA
//	float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
	Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
	Ogre::Real zoom = 0.0f;

	// HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
	// ARROWS TO MOVE THE CAMERA
	// NOTE: we are getting the cameraPosition and modifying the position
	// without using translation, this is because we want to move always
	// in the same axis whatever be the direction of the camera.


	// MOUSE
	const OIS::MouseState& lMouseState = GLOBAL_MOUSE->getMouseState();
	mMouseCursor.updatePosition(lMouseState.X.abs, lMouseState.Y.abs);

}

Test::Test()
{
//	setUseDefaultInput(false);
	mMouseCursor.setVisible(true);
	mMouseCursor.setWindowDimensions(GLOBAL_WINDOW->getWidth(), GLOBAL_WINDOW->getHeight());


	//We put the cursor in the middle of the screen
	OIS::MouseState &mMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
	mMouseState.X.abs = mCamera->getViewport()->getActualWidth() / 2;
	mMouseState.Y.abs = mCamera->getViewport()->getActualHeight() / 2;

	mMouseCursor.updatePosition(mMouseState.X.abs,mMouseState.Y.abs);

}

Test::~Test()
{
	// TODO Auto-generated destructor stub
}


/* Load additional info */
void Test::loadAditionalData(void)
{
	mBbBatery.createSet(20, "Billboard/ZombieBlood", 10);
	mBbBatery.setVisible(true);

	mBbManager.createSet(20, "HUD/ExtraAction/bombs", 3, 3);
	mBbManager.setVisible(true);
	Ogre::AxisAlignedBox bb;
	bb.setMaximum(99999,99999,99999);
	bb.setMinimum(-99999,-99999,-99999);
	mBbManager.setBounds(bb,999999);
}

/* function called every frame. Use GlobalObjects::lastTimeFrame */
void Test::update()
{
	handleInput();


	static bool mousePressed = false;
	// Mouse input check
	if(GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MB_Left)){
		if (!mousePressed)
		{
			mousePressed = true;

			Ogre::Vector3 pos;
			if(!performMouseRay(pos))
				return;

			// else we generate a blood in that position
			pos.y += 1.0f;
//			mBbBatery.showBillboard(pos);
			static int auxp = 0;
			if(auxp == 3){
				debugRED("NO se puede crear mas \n");
				return;
			}
			// create a billboard
			effects::BillboardWrapper bb = mBbManager.getNewBillboard(auxp);
			auxp++;
			ASSERT(bb);
			debugGREEN("Billboard created on (%f,\t%f,\t%f)\n", pos.x,pos.y,pos.z);
			bb->setPosition(pos);

		}
	} else {
		mousePressed = false;
	}

}
