/*
 * CameraController.cpp
 *
 *  Created on: 23/01/2012
 *      Author: agustin
 */
#include <auto_ptr.h>

#include "CameraController.h"
#include "Util.h"

#include "DebugUtil.h"


////////////////////////////////////////////////////////////////////////////////

void
CameraController::CameraState::setNodes(Ogre::SceneNode *rootNode,
                                        Ogre::SceneNode *rotNode,
                                        Ogre::SceneNode *camNode)
{
    ASSERT(rootNode);
    ASSERT(rotNode);
    ASSERT(camNode);

    mRootNode = rootNode;
    mRotationNode = rotNode;
    mCameraNode = camNode;
}

void
CameraController::CameraState::saveState(void)
{
    mPositions[Index::RootNode] = mRootNode->getPosition();
    mOrientations[Index::RootNode] = mRootNode->getOrientation();

    mPositions[Index::RotationNode] = mRotationNode->getPosition();
    mOrientations[Index::RotationNode] = mRotationNode->getOrientation();

    mPositions[Index::CameraNode] = mCameraNode->getPosition();
    mOrientations[Index::CameraNode] = mCameraNode->getOrientation();
}
void
CameraController::CameraState::restoreState(void)
{
    mRootNode->setPosition(mPositions[Index::RootNode]);
    mRootNode->setOrientation(mOrientations[Index::RootNode]);

    mRotationNode->setPosition(mPositions[Index::RotationNode]);
    mRotationNode->setOrientation(mOrientations[Index::RotationNode]);

    mCameraNode->setPosition(mPositions[Index::CameraNode]);
    mCameraNode->setOrientation(mOrientations[Index::CameraNode]);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateCameraDown(Ogre::Camera *cam)
{
	ASSERT(cam);
	cam->setOrientation(Ogre::Quaternion(180,1,0,0));

}

////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateMaxXCamera(void)
{
	const Ogre::Radian rot = mMaxPitchRot - mCameraNode->getOrientation().getPitch();
//	rotateCameraX(rot);
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::rotateMinXCamera(void)
{
//	const Ogre::Radian rot = mCameraNode->getOrientation().getPitch();
//	rotateCameraX(mMinPitchRot);
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
mRotVelocity(0.5f),
mMinPitchRot(-0.67305f),
mMaxPitchRot(0.5),
mZoom(75.0f),                    // start with the 75 % of the zoom,
mInternalState(State::Normal)
{
	// create the scene node
	mRootNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode("CameraControllerNode");
	mRotationNode = mRootNode->createChildSceneNode();
	mCameraNode = mRotationNode->createChildSceneNode(
			Ogre::Vector3(0,NODE_DISTANCE_Y,NODE_DISTANCE_Z));
	mCameraNode->lookAt(mRootNode->getPosition(),Ogre::Node::TS_WORLD);
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
	const Ogre::Vector3 distVec = mRootNode->getPosition() - mCameraNode->getPosition();
	mStartDistance = distVec.length();
	mCamState.setNodes(mRootNode, mRotationNode, mCameraNode);
	mUpdater.setSceneNode(mCameraNode);

	// configure the zoom
	zoomCamera(mZoom);
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
    if (zoom > MAX_ZOOM || zoom < MIN_ZOOM || mInternalState != State::Normal) {
        return;
    }
    Ogre::Vector3 posVec = mCameraNode->getPosition();
    posVec.normalise();
    const Ogre::Real len = zoom * mStartDistance * 0.01f;
    mCameraNode->setPosition(posVec * len);
    mZoom = zoom;
//
//	mNextPosition = mRootNode->getPosition();
//	mNextPosition.y += zoom * mCamVelocityFactor *
//			GLOBAL_TIME_FRAME * COEFF_FACTOR;
//	if(mMoveBox.contains(mNextPosition)){
//		mRootNode->setPosition(mNextPosition);
//	}

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
void CameraController::setCameraOrientation(const Ogre::Quaternion &o)
{
	const Ogre::Radian rot = o.getPitch();
//	rotateCameraX(rot);
	const Ogre::Radian roty = o.getYaw();
//	rotateCameraY(roty);
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

	if (mInternalState == State::SatelliteMode) {
	    return;
	}
	mInternalState = State::SatelliteMode;

	// backup the position and orientation
	mCamState.saveState();

	// set the camera position just with the root node
	mCameraNode->setPosition(Ogre::Vector3::ZERO);

	// get the middle height of the box and move the camera up
	const Ogre::Real minY = mMoveBox.getHalfSize().y;
	Ogre::Vector3 npos = getCamPos();
	npos.y = minY;
	setCamPos(npos);

    // rotate the camera to look down
    npos.y -= 100.0f;
    mCameraNode->lookAt(npos, Ogre::Node::TS_WORLD);
}

////////////////////////////////////////////////////////////////////////////////
void CameraController::restoreBeforeSatellite(void)
{
    if (mInternalState == State::Normal) {
        return;
    }
    mInternalState = State::Normal;

    // restore the camera values
    mCamState.restoreState();

    debugERROR("Probablemente esto no sea un error, pero no esta funcionando "
        "el hecho de volver a poner la camara en el angulo (X-axis) que "
        "estaba en el backup...\n");
}

