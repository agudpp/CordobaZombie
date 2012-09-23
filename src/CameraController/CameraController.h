/*
 * CameraController.h
 *
 *  Created on: 23/01/2012
 *      Author: agustin
 */

#ifndef CAMERACONTROLLER_H_
#define CAMERACONTROLLER_H_

#include <OgreCamera.h>
#include <OgreSceneNode.h>

#include "CameraAnimUpdater.h"

#define CAMERA_ANIMATIONS_FILE		"cameraAnims.xml"

class CameraController
{
	static constexpr Ogre::Real COEFF_FACTOR	= 200.0f;
	// the distance from the rotation axis
	static constexpr Ogre::Real NODE_DISTANCE	= 130.0f;

public:

	// Animations
	enum {
		ANIM_VIBRATION_H = 0,
		ANIM_VIBRATION_M,
		ANIM_VIBRATION_L,
	};

public:
	CameraController(Ogre::Camera *cam = 0);
	~CameraController();

	// Sets the camera
	void setCamera(Ogre::Camera *);

	// Release the camera
	void release(void);

	// Returns the camera
	Ogre::Camera *getCamera(void) const {return mCamera;}

	//				CONFIGURE THE CAMERA				//

	// Set the bounding box where the camera can move
	void setCameraMoveZone(const Ogre::AxisAlignedBox &zone);

	// Set maximum angle that the camera can rotate over the X axis
	void setMaxRotateX(const Ogre::Radian &max, const Ogre::Radian &min);

	// Set/Get the velocity factor of the camera movement
	void setCameraVelocity(Ogre::Real v) {mCamVelocityFactor = v;}
	Ogre::Real getCameraVelocity(void) const {return mCamVelocityFactor;}

	// Set/Get RotationVelocity
	void setRotationVelocity(Ogre::Real v) {mRotVelocity = v;}
	Ogre::Real getRotationVelocity(void) const {return mRotVelocity;}

	// 				USE THE CAMERA 						//

	// Move the camera in axis x,z
	void moveCamera(const Ogre::Vector3 &dir);

	// Zoom the camera
	void zoomCamera(const Ogre::Real zoom);

	// Sets the camera position
	void setCamPos(const Ogre::Vector3 &pos);
	inline const Ogre::Vector3 &getCamPos(void) const;

	// TODO: Rotate the camera over the Y Axis
	void rotateCameraY(const Ogre::Radian &r);

	// TODO: Rotate the camera over the X Axis
	void rotateCameraX(const Ogre::Radian &r);

	// set the rotation of x
	void setRotationX(const Ogre::Radian &r);
	void setRotationY(const Ogre::Radian &r);

	// Set the camera orientation
	void setCameraOrientation(const Ogre::Quaternion &o);
	inline const Ogre::Quaternion &getOrientation(void) const;

	//							EFFECTS										//

	/**
	 * Reproduce one of the camera animations
	 */
	void reproduceAnimation(int anim);
	void stopAnimation(void);

	/**
	 * Set the position to be vertically to the floor. This will put the
	 * camera as "Satellite" mode.
	 */
	void setSatellitePosition(void);

	/**
	 * Restore angle and Height of the camera that have before we call
	 * "setSatellitePosition"
	 */
	void restoreBeforeSatellite(void);


private:
	// Set the camera to show down
	void rotateCameraDown(Ogre::Camera *cam);
	void rotateMaxXCamera(void);
	void rotateMinXCamera(void);

	/**
	 * Load the animations
	 */
	bool loadAnimations(void);



private:
	// Camera
	Ogre::Camera			*mCamera;
	Ogre::SceneNode			*mRootNode;
	Ogre::SceneNode			*mRotationNode;
	Ogre::SceneNode			*mCameraNode;

	Ogre::AnimationState	*mAnimations[3];

	CameraAnimUpdater		mUpdater;

	// Camera movement config
	Ogre::AxisAlignedBox	mMoveBox;
	Ogre::Radian			mMaxXRotation;
	Ogre::Radian			mMinXRotation;
	Ogre::Vector3			mNextPosition;
	Ogre::Real				mCamVelocityFactor;
	Ogre::Real				mRotVelocity;

	// Backup info
	Ogre::Radian			mBackupRotX;
	Ogre::Radian			mBackupRotY;
	Ogre::Vector3			mBackupPosition;

};




inline const Ogre::Vector3 &CameraController::getCamPos(void) const
{
	return mRootNode->getPosition();
}

inline const Ogre::Quaternion &CameraController::getOrientation(void) const
{
	return mCameraNode->getOrientation();
}

#endif /* CAMERACONTROLLER_H_ */
