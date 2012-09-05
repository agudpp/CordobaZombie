/*
 * Fade.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef FADE_H_
#define FADE_H_

#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreTextureUnitState.h>

#include "OverlayEffect.h"
#include "DebugUtil.h"

namespace OvEff {

class Fade : public OverlayEffect {
public:
	enum Type {
		FADE_IN,
		FADE_OUT
	};
public:

	/**
	 * This effect will automatically create the OverlayElement to be used.
	 * By now we do not support to use other overlay (don't call the function
	 * setElement())
	 */
	Fade();
	virtual ~Fade();

	/**
	 * Set the time duration of the transition
	 * @param	t	Transition time duration
	 */
	inline void setTime(float t);
	inline float getTime(void) const;

	/**
	 * Set the type of effect
	 * @param	t	The functionality type
	 */
	inline void setType(Type t);
	inline Type getType(void) const;

	/**
	 * Configure the size and position using other element
	 * @param	e	The element that we want to "fade"
	 */
	inline void configureFade(Ogre::OverlayElement *e);

	/**
	 * Configure using a "manual" size (between [0,1]).
	 * @param	t	Top
	 * @param	l	Left
	 * @param	b	Bottom
	 * @param	r	Right
	 */
	inline void configureFace(Ogre::Real t, Ogre::Real l, Ogre::Real b, Ogre::Real r);

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
	Type					mType;
	float					mTime;
	Ogre::Real				mAccumTime;
	Ogre::TextureUnitState	*mTexture;

	static Ogre::Overlay	*mOverlay;
	static unsigned int		mICounter;
};


////////////////////////////////////////////////////////////////////////////////
inline void Fade::setTime(float t)
{
	mTime = t;
}
inline float Fade::getTime(void) const
{
	return mTime;
}

////////////////////////////////////////////////////////////////////////////////
inline void Fade::setType(Fade::Type t)
{
	mType = t;
}
inline Fade::Type Fade::getType(void) const
{
	return mType;
}

////////////////////////////////////////////////////////////////////////////////
inline void Fade::configureFade(Ogre::OverlayElement *e)
{
	ASSERT(e);
	ASSERT(mElement);
	mElement->setDimensions(e->getWidth(), e->getHeight());
	mElement->setPosition(e->getLeft(), e->getTop());
	mElement->setMetricsMode(e->getMetricsMode());
}

inline void Fade::configureFace(Ogre::Real t, Ogre::Real l,
		Ogre::Real b, Ogre::Real r)
{
	ASSERT(mElement);
	ASSERT(t >= 0.0f && t <= 1.0f);
	ASSERT(b >= 0.0f && b <= 1.0f);
	ASSERT(r >= 0.0f && r <= 1.0f);
	ASSERT(l >= 0.0f && tl <= 1.0f);
	ASSERT(b >= t);
	ASSERT(r >= l);

	mElement->setDimensions(r-l, b-t);
	mElement->setPosition(l, t);
}

}

#endif /* FADE_H_ */
