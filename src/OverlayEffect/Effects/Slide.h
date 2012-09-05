/*
 * Slide.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef SLIDE_H_
#define SLIDE_H_

#include <OgreVector2.h>

#include "OverlayEffect.h"

namespace OvEff {


class Slide : public OverlayEffect {
public:
	/**
	 * We will use a class to represent the change of velocity that we want
	 * to use to translate the overlays in the windows.
	 * If we use for example a constant function then we will translate the
	 * overlayElement at the same velocity all the time
	 * The function will take a value between [0,1] representing the position
	 * of all the translation movement (0 mean the Origin point, and 1 means the
	 * Destiny point).
	 */
	class VelFunction {
	public:
		virtual float operator()(float x)
		{
			return 1.0f;
		}
	};

public:
	Slide();
	virtual ~Slide();

	/**
	 * Set the translation positions
	 * @param orig	The begin position
	 * @param dest	The destination position
	 */
	inline void setTranslationPositions(const Ogre::Vector2 &orig,
			const Ogre::Vector2 &dest);

	/**
	 * Set the velocity (acceleration) function to be used
	 * @param	f	The function
	 */
	inline void setFunction(const VelFunction &f);

protected:
	/**
	 * Functions to be implemented by the user when the manager will start to
	 * execute the effect (before the update cycle)
	 */
	virtual void enter(void);

	/**
	 * Updating function to be implemented by the user (updated by the manager)
	 */
	virtual void update(void);

	/**
	 * Function called by the manager when the effect stops executing
	 */
	virtual void exit(void);

private:
	Ogre::Vector2		mOrig;
	Ogre::Vector2		mDest;
	Ogre::Vector2		mTransVec;
	Ogre::Real			mInvTotalSqrLen;
	Ogre::Vector2		mActualPos;
	VelFunction			mFun;


};




////////////////////////////////////////////////////////////////////////////////
inline void Slide::setTranslationPositions(const Ogre::Vector2 &orig,
		const Ogre::Vector2 &dest)
{
	mOrig = orig;
	mDest = dest;
	mTransVec = dest-orig;
	const Ogre::Real l = mTransVec.normalise();
	mInvTotalSqrLen = 1.0f/(l*l);
}

////////////////////////////////////////////////////////////////////////////////
inline void Slide::setFunction(const VelFunction &f)
{
	mFun = f;
}


}

#endif /* SLIDE_H_ */
