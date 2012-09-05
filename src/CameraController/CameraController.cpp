/*
 * CameraController.cpp
 *
 *  Created on: 23/01/2012
 *      Author: agustin
 */
#include <auto_ptr.h>

#include "CameraController.h"
#include "GlobalObjects.h"
#include "Util.h"

#include "DebugUtil.h"


////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateCameraDown(Ogre::Camera *cam)
{
	ASSERT(cam);
	cam->setOrientation(Ogre::Quaternion(180,1,0,0));

}

////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateMaxXCamera(void)
{
	const Ogre::Radian rot = mMaxXRotation - mCameraNode->getOrientation().getPitch();
	rotateCameraX(rot);
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateMinXCamera(void)
{
//	const Ogre::Radian rot = mCameraNode->getOrientation().getPitch();
	rotateCameraX(mMinXRotation);
}


////////////////////////////////////////////////////////////////////////////////
bool CameraController::loadAnimations(void)
{
	std::auto_ptr<TiXmlDocument> doc(Common::Util::loadXmlDocument(CAMERA_ANIMATIONS_FILE));
	if(!doc.get()){
		debugRED("Error loading the animation for the camera %s not found\n",
				CAMERA_ANIMATIONS_FILE);
		ASSERT(false);
		return false;
	}
	TiXmlElement *animation = doc->RootElement();
	if(!animation || !(animation = animation->FirstChildElement("animations"))){
		debugRED("Error: No animation found in the xml\n");;
		ASSERT(false);
		return false;
	}

	// Now parse the animations
	std::list<Ogre::AnimationState *> animList;
	if(!Common::Util::getAnimation(GLOBAL_SCN_MNGR,
			mCameraNode,
			animation,
			animList)){
		debug("Error: Some error occur when loading the animations of the player head\n");
		ASSERT(false);
		return false;
	}

	// now we have the head anim, really check that
	ASSERT(GLOBAL_SCN_MNGR->hasAnimationState("VIBRATION_H"));
	ASSERT(GLOBAL_SCN_MNGR->hasAnimationState("VIBRATION_M"));
	ASSERT(GLOBAL_SCN_MNGR->hasAnimationState("VIBRATION_L"));

	// now that we have, we put that animations in the array
	mAnimations[ANIM_VIBRATION_H] = GLOBAL_SCN_MNGR->getAnimationState("VIBRATION_H");
	mAnimations[ANIM_VIBRATION_M] = GLOBAL_SCN_MNGR->getAnimationState("VIBRATION_M");
	mAnimations[ANIM_VIBRATION_L] = GLOBAL_SCN_MNGR->getAnimationState("VIBRATION_L");

	return true;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
CameraController::CameraController(Ogre::Camera *cam) :
mCamera(0),
mCameraNode(0),
mRootNode(0),
mRotationNode(0),
mCamVelocityFactor(1.0f),
mRotVelocity(0.005f),
mMinXRotation(-0.67305f),
mMaxXRotation(0)
{
	// create the scene node
	mRootNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode("CameraControllerNode");
	mRotationNode = mRootNode->createChildSceneNode();
	mCameraNode = mRotationNode->createChildSceneNode(
			Ogre::Vector3(0,0,NODE_DISTANCE));
	if(mCamera){
		if(mCamera->getParentSceneNode()){
			mCamera->getParentSceneNode()->detachObject(mCamera);
		}
		rotateCameraDown(mCamera);
		mCameraNode->attachObject(mCamera);
	}

	if(!loadAnimations()){
		debugRED("Error loading the animations!\n");
		ASSERT(false);
	}

	mUpdater.setSceneNode(mCameraNode);
}

////////////////////////////////////////////////////////////////////////////////
CameraController::~CameraController()
{
	mCameraNode->detachAllObjects();
	mRootNode->removeAndDestroyAllChildren();
	GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(mRootNode->getName());

}


////////////////////////////////////////////////////////////////////////////////
void CameraController::setCamera(Ogre::Camera *c)
{
	ASSERT(c);
	ASSERT(!c->getParentSceneNode());

	mCamera = c;
	rotateMaxXCamera();
	if(mCamera->getParentSceneNode()){
		mCamera->getParentSceneNode()->detachObject(mCamera);
	}
	mCameraNode->attachObject(mCamera);
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::release(void)
{
	if(!mCamera || !mCamera->getParentSceneNode()) return;
	mCamera->getParentSceneNode()->detachObject(mCamera);
}


//				CONFIGURE THE CAMERA				//

////////////////////////////////////////////////////////////////////////////////
void CameraController::setCameraMoveZone(const Ogre::AxisAlignedBox &zone)
{
	mMoveBox = zone;
	if(!mMoveBox.contains(mRootNode->getPosition())){
		mRootNode->setPosition(mMoveBox.getCenter());
	}
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::setMaxRotateX(const Ogre::Radian &max, const Ogre::Radian &min)
{
	mMaxXRotation = max;
	mMinXRotation = min;

	Ogre::Radian r = mRotationNode->getOrientation().getPitch();
	if(r >= mMaxXRotation || r <= mMinXRotation){
		mRotationNode->setOrientation(Ogre::Quaternion(r,
				Ogre::Vector3::UNIT_X));
	}
}

// 				USE THE CAMERA 						//

////////////////////////////////////////////////////////////////////////////////
void CameraController::moveCamera(const Ogre::Vector3 &dir)
{
	mNextPosition = mRootNode->getPosition() +
			mRootNode->getOrientation() * (dir *(mCamVelocityFactor *
					GLOBAL_TIME_FRAME * COEFF_FACTOR));
	if(mMoveBox.contains(mNextPosition)){
		// move the camera
		mRootNode->setPosition(mNextPosition);
	} else {
		// do nothing
		return;
	}

}

////////////////////////////////////////////////////////////////////////////////
void CameraController::zoomCamera(const Ogre::Real zoom)
{
	mNextPosition = mRootNode->getPosition();
	mNextPosition.y += zoom * mCamVelocityFactor *
			GLOBAL_TIME_FRAME * COEFF_FACTOR;
	if(mMoveBox.contains(mNextPosition)){
		mRootNode->setPosition(mNextPosition);
	}

}

////////////////////////////////////////////////////////////////////////////////
void CameraController::setCamPos(const Ogre::Vector3 &pos)
{
	if(mMoveBox.contains(pos)){
		mRootNode->setPosition(pos);
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateCameraY(const Ogre::Radian &r)
{
	mRootNode->rotate(Ogre::Vector3::UNIT_Y, r * GLOBAL_TIME_FRAME *
			COEFF_FACTOR * mRotVelocity, Ogre::Node::TS_LOCAL);
//	mCameraNode->yaw(mCameraNode->getOrientation().getYaw() + r);
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateCameraX(const Ogre::Radian &r)
{
	const Ogre::Radian pitch = mRotationNode->getOrientation().getPitch();
	Ogre::Radian newR = (r * GLOBAL_TIME_FRAME *
			COEFF_FACTOR * mRotVelocity) + pitch;

	if(newR <= mMaxXRotation && newR >= mMinXRotation){
		newR -= pitch;
		mRotationNode->rotate(Ogre::Vector3::UNIT_X, newR, Ogre::Node::TS_LOCAL);
	} else {
		// do nothing
		return;
	}

}

////////////////////////////////////////////////////////////////////////////////
void CameraController::setRotationX(const Ogre::Radian &r)
{
	if(r <= mMaxXRotation && r >= mMinXRotation){
		mRotationNode->setOrientation(Ogre::Quaternion(r, Ogre::Vector3::UNIT_X));
	}
}
////////////////////////////////////////////////////////////////////////////////
void CameraController::setRotationY(const Ogre::Radian &r)
{
	mRootNode->setOrientation(Ogre::Quaternion(r, Ogre::Vector3::UNIT_Y));
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::setCameraOrientation(const Ogre::Quaternion &o)
{
	const Ogre::Radian rot = o.getPitch();
	rotateCameraX(rot);
	const Ogre::Radian roty = o.getYaw();
	rotateCameraY(roty);
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::reproduceAnimation(int anim)
{
	ASSERT(anim >= 0 && anim < 3);
	mUpdater.startAnimation(mAnimations[anim]);
}
////////////////////////////////////////////////////////////////////////////////
void CameraController::stopAnimation(void)
{
	mUpdater.stopUpdate();
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::setSatellitePosition(void)
{
	debugWARNING("Aca estamos poniendo la mitad de la altura del bounding "
			"box por el cual nos podemos mover, capaz que convenga algo mas"
			" alto?\n");

	// backup the position and orientation
	mBackupPosition = getCamPos();
	mBackupRotX = getOrientation().getPitch();
	mBackupRotY = getOrientation().getYaw();

	// get the middle height
	const Ogre::Real minY = mMoveBox.getHalfSize().y;
	Ogre::Vector3 npos = getCamPos();
	npos.y = minY;
	setCamPos(npos);

	// set the new orientation
	rotateMinXCamera();
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::restoreBeforeSatellite(void)
{
	// update only the y vector;
	Ogre::Vector3 p = getCamPos();
	p.y = mBackupPosition.y;
	setCamPos(p);

	debugERROR("Probablemente esto no sea un error, pero no esta funcionando "
			"el hecho de volver a poner la camara en el angulo (X-axis) que "
			"estaba en el backup...\n");
	setRotationX(mBackupRotX);
	setRotationY(mBackupRotY);
}

