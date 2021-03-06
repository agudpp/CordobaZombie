/*
 * OverlayEffect.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef OVERLAYEFFECT_H_
#define OVERLAYEFFECT_H_

#include <OgreOverlayElement.h>

#include "EffectCb.h"

namespace OvEff {

class OverlayEffectManager;

class OverlayEffect {
public:
	OverlayEffect();
	virtual ~OverlayEffect();

	/**
	 * Set the Overlay effect manager to be used
	 * @param	em	the OverlayEffectManager
	 */
	static void setManager(OverlayEffectManager *em);

	/**
	 * Set/Get the OverlayElement to be used
	 * @param	e	The overlay element
	 */
	inline void setElement(Ogre::OverlayElement *e);
	inline Ogre::OverlayElement *getElement(void);

	/**
	 * Set the callback to be used to call when the effect will start to be
	 * executed or when its ending
	 * @param	cb	The callback
	 */
	inline void setCallback(EffectCb *cb);

	/**
	 * Stops the effect (if is actually reproducing it
	 * @note Removes the effect automatically from the manager
	 */
	void stop(void);

	/**
	 * Start the effect.
	 * @note	Put the effect in the Manager
	 */
	void start(void);

protected:

	/**
	 * Functions to be implemented by the user when the manager will start to
	 * execute the effect (before the update cycle)
	 */
	virtual void enter(void) = 0;

	/**
	 * Updating function to be implemented by the user (updated by the manager)
	 */
	virtual void update(void) = 0;

	/**
	 * Function called by the manager when the effect stops executing
	 */
	virtual void exit(void) = 0;

protected:
	friend class OverlayEffectManager;
	/**
	 * Functions called by the EffectManager
	 */
	inline void begin(void);
	inline void end(void);
	inline void setIndex(int i);
	inline int getIndex(void) const;



protected:
	Ogre::OverlayElement			*mElement;
	EffectCb						*mCb;

	// pointer (index) used by the Manager
	int								mIndex;

	static OverlayEffectManager		*mMngr;
};



/**
 * Set/Get the OverlayElement to be used
 * @param	e	The overlay element
 */
inline void OverlayEffect::setElement(Ogre::OverlayElement *e)
{
	mElement = e;
}
inline Ogre::OverlayElement *OverlayEffect::getElement(void)
{
	return mElement;
}

/**
 * Set the callback to be used to call when the effect will start to be
 * executed or when its ending
 * @param	cb	The callback
 */
inline void OverlayEffect::setCallback(EffectCb *cb)
{
	mCb = cb;
}

/**
 * Functions called by the EffectManager
 */
inline void OverlayEffect::begin(void)
{
	if(mCb) {(*mCb)(EffectCb::STARTING);}
	enter();
}
inline void OverlayEffect::end(void)
{
	if(mCb) {(*mCb)(EffectCb::ENDING);}
	exit();
}
inline void OverlayEffect::setIndex(int i)
{
	mIndex = i;
}
inline int OverlayEffect::getIndex(void) const
{
	return mIndex;
}



}

#endif /* OVERLAYEFFECT_H_ */
