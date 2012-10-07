/*
 * OverlayEffect.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef OVERLAYEFFECT_H_
#define OVERLAYEFFECT_H_

#include <algorithm>
#include <vector>

#include <OgreOverlayElement.h>


#include "EffectCb.h"
#include "DebugUtil.h"

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
	virtual void setElement(Ogre::OverlayElement *e);
	inline Ogre::OverlayElement *getElement(void);

	/**
	 * Add the callback to be used to call when the effect will start to be
	 * executed or when its ending
	 * @param	cb	The callback
	 */
	inline void addCallback(EffectCb *cb);
	inline void removeCallback(EffectCb *cb);

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

	/**
	 * Check if the effect is active
	 */
	inline bool isActive(void) const;

	/**
	 * @brief
	 * Turns the effect into its complement.
	 * v.gr. a FADE_IN fade effect is changed into a FADE_OUT fade effect
	 *
	 * @return
	 * true		the effect was changed into its complement
	 * false	the effect has no complement, and was not changed
	 */
	virtual bool complement() = 0;

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
	typedef std::vector<EffectCb *> EffectCbVec;

	Ogre::OverlayElement			*mElement;
	bool                            mActive;
	EffectCbVec                     mCallbacks;
	// pointer (index) used by the Manager
	int								mIndex;

	static OverlayEffectManager		*mMngr;
};



inline Ogre::OverlayElement *OverlayEffect::getElement(void)
{
	return mElement;
}

/**
 * Set the callback to be used to call when the effect will start to be
 * executed or when its ending
 * @param	cb	The callback
 */
inline void OverlayEffect::addCallback(EffectCb *cb)
{
    ASSERT(std::find(mCallbacks.begin(), mCallbacks.end(), cb) == mCallbacks.end());
    ASSERT(cb != 0);
    // increment only in 1 the size
    mCallbacks.reserve(mCallbacks.size() + 1);
    mCallbacks.push_back(cb);
}
inline void OverlayEffect::removeCallback(EffectCb *cb)
{
    ASSERT(cb != 0);
    for(size_t size = mCallbacks.size(), index = 0; index < size; ++index)
        if (mCallbacks[index] == cb){
            // swap with the last element
            mCallbacks[index] = mCallbacks[size-1];
            mCallbacks.pop_back();
            return;
        }
}

/**
 * Functions called by the EffectManager
 */
inline void OverlayEffect::begin(void)
{
	if(!mCallbacks.empty()) {
	    for(size_t size = mCallbacks.size(), i = 0; i < size; ++i)
	        (*(mCallbacks[i]))(EffectCb::STARTING);
	}
	enter();
	mActive = true;
}
inline void OverlayEffect::end(void)
{
    if(!mCallbacks.empty()) {
        for(size_t size = mCallbacks.size(), i = 0; i < size; ++i)
            (*(mCallbacks[i]))(EffectCb::ENDING);
    }
	exit();
	mActive = false;
}
inline void OverlayEffect::setIndex(int i)
{
	mIndex = i;
}
inline int OverlayEffect::getIndex(void) const
{
	return mIndex;
}
inline bool
OverlayEffect::isActive(void) const
{
    return mActive;
}


}

#endif /* OVERLAYEFFECT_H_ */
