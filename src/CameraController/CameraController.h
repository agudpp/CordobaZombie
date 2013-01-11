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
#include "GlobalObjects.h"

#define CAMERA_ANIMATIONS_FILE		"cameraAnims.xml"

class CameraController
{
	static const Ogre::Real COEFF_FACTOR	= 200.0f;
	// the distance from the rotation axis
	static const Ogre::Real NODE_DISTANCE_Z	= 130.0f;
	static const Ogre::Real NODE_DISTANCE_Y	= 130.0f;
	static const Ogre::Real MIN_ZOOM = 20.0f;
	static const Ogre::Real MAX_ZOOM = 100.0f;

	// The internal state of the CameraController
	enum State {
	    Normal = 0,
	    SatelliteMode,
	    ExternalAnimation,
	};


	// Auxiliar class to save the positions + orientations of the camera scene
	// nodes and then to restore it
	class CameraState {
	    enum Index {
	        RootNode = 0,
	        RotationNode,
	        CameraNode
	    };
	public:
	    void setNodes(Ogre::SceneNode *rootNode,
                      Ogre::SceneNode *rotNode,
                      Ogre::SceneNode *camNode);

	    void saveState(void);
	    void restoreState(void);

	private:
	    Ogre::SceneNode         *mRootNode;
        Ogre::SceneNode         *mRotationNode;
        Ogre::SceneNode         *mCameraNode;

        Ogre::Vector3 mPositions[3];
        Ogre::Quaternion mOrientations[3];
	};

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

	// Returns the camera scene node
	//
	Ogre::SceneNode *getCameraSceneNode(void) const {return mCameraNode;}

	//				CONFIGURE THE CAMERA				//

	// Set the bounding box where the camera can move
	void setCameraMoveZone(const Ogre::AxisAlignedBox &zone);

	// Set the pitch (Y) rotation range
	//
	void setPitchRange(const Ogre::Radian min, const Ogre::Radian max);

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
	inline Ogre::Real zoom(void) const {return mZoom;}

	// Sets the camera position
	void setCamPos(const Ogre::Vector3 &pos);
	inline const Ogre::Vector3 &getCamPos(void) const;

	// Rotate the camera using absolute Radian degree.
	//
	inline void rotateCamera(const Ogre::Radian yaw, const Ogre::Radian pitch);

	// Get the rotations values from the camera
	inline void getRotations(Ogre::Radian &yaw, Ogre::Radian &pitch);

	// Set the camera orientation
	void setCameraOrientation(const Ogre::Quaternion &o);
	inline const Ogre::Quaternion &getOrientation(void) const;

	//							EFFECTS										//

	/**
	 * Reproduce one of the camera animations
	 */
	void reproduceAnimation(int anim);

	/**
	 * Reproduce an external animation, The camera controller is not the owner
	 * of the animation so it should be removed after the camera stop using it
	 */
	void reproduceAnimation(Ogre::AnimationState *anim);

	/**
	 * Stop the actual animation...
	 */
	void stopAnimation(void);

	/**
	 * Check if we are currently running any animation
	 */
	inline const bool isAnimationRunning(void) const;

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

	/**
	 * Load the animations
	 */
	bool loadAnimations(void);

	/**
	 * Callback received for when the CameraAnimUpdater finish its animation
	 */
	void animFinishedCb(void);



private:
	// Camera
	Ogre::Camera			*mCamera;
	Ogre::SceneNode			*mRootNode;
	Ogre::SceneNode			*mRotationNode;
	Ogre::SceneNode			*mCameraNode;

	Ogre::AnimationState	*mAnimations[3];
	Ogre::AnimationState	*mExternalAnimation;

	CameraAnimUpdater		mUpdater;
	CameraAnimUpdater::Connection mCbConn;
	bool mIsAnimRunning;

	// Camera movement config
	Ogre::AxisAlignedBox	mMoveBox;
	Ogre::Radian			mMaxPitchRot;
	Ogre::Radian			mMinPitchRot;
	Ogre::Vector3			mNextPosition;
	Ogre::Real				mCamVelocityFactor;
	Ogre::Real				mRotVelocity;
	Ogre::Real              mStartDistance;
	Ogre::Real              mZoom;

	// Backup info
	CameraState mCamState;
	State mInternalState;

};




inline const Ogre::Vector3 &CameraController::getCamPos(void) const
{
	return mRootNode->getPosition();
}

inline const Ogre::Quaternion &CameraController::getOrientation(void) const
{
	return mCameraNode->getOrientation();
}

inline void
CameraController::rotateCamera(const Ogre::Radian yaw, const Ogre::Radian pitch)
{
    if (mInternalState != State::Normal) {
        return;
    }
    mRootNode->yaw(yaw , Ogre::Node::TS_WORLD);
    const Ogre::Radian futurePitch = mRotationNode->getOrientation().getPitch() +
            pitch;
    if (futurePitch > mMaxPitchRot || futurePitch < mMinPitchRot) {
        return;
    }
    mRotationNode->pitch(pitch);
}

inline void
CameraController::getRotations(Ogre::Radian &yaw, Ogre::Radian &pitch)
{
    yaw = mRootNode->getOrientation().getYaw();
    pitch = mRotationNode->getOrientation().getPitch();
}

inline const bool
CameraController::isAnimationRunning(void) const
{
    return mIsAnimRunning;
}

#endif /* CAMERACONTROLLER_H_ */
