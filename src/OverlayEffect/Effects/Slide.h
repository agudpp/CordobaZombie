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
#include "DebugUtil.h"

namespace OvEff {


class Slide : public OverlayEffect {
public:
	/**
	 * We will use a class to represent the change of velocity that we want
	 * to use to translate the overlays in the windows.
	 * If we use for example a constant function then we will translate the
	 * overlayElement at the same velocity all the time
	 * The function will take a value between [0,1] representing the percentage
	 * of accumulated time from the start of the translation.
	 * It will return a value also between [0,1] representing the percentage of
	 * translation from the start point corresponding to the curve defined by
	 * this velocity function.
	 */
	class VelFunction {
	public:
		virtual float operator()(float x) const
		{
			return 1.0f;
		}
	};

	// Some Useful functions here
	static const VelFunction *LINEAL_FUNCTION;

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
	inline void setFunction(const VelFunction *f);

	/**
	 * Set the total time that the slide should last
	 * t must be grater than 0.0f.
	 */
	inline void setDuration(float t);

	/**
	 * @brief
	 * Turns the effect into its complement
	 * i.e., the origin and destiny positions are swapped.
	 *
	 * @return
	 * true		the effect was changed into its complement
	 */
	inline virtual bool complement();

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
	Ogre::Vector2		mActualPos;
	const VelFunction	*mFun;
	// 1.0f / total time that should last the slide movement:
	float				mTimeLapse;
	// Accumulated time lapsed in the slide movement:
	float				mAccumTime;

};




////////////////////////////////////////////////////////////////////////////////
inline void Slide::setTranslationPositions(const Ogre::Vector2 &orig,
		const Ogre::Vector2 &dest)
{
	mOrig = orig;
	mDest = dest;
	mTransVec = dest-orig;
}

////////////////////////////////////////////////////////////////////////////////
inline void Slide::setFunction(const VelFunction *f)
{
	mFun = f;
}


////////////////////////////////////////////////////////////////////////////////
inline void Slide::setDuration(float t)
{
	ASSERT(t > 0.0f);
	mTimeLapse = 1.0f/t;
}


////////////////////////////////////////////////////////////////////////////////
inline virtual bool Slide::complement()
{
	Ogre::Vector2 tmp(mOrig);
	mOrig = mDest;
	mDest = tmp;
	return true;
}

}

#endif /* SLIDE_H_ */
