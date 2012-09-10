/*
 * Alpha.h
 *
 *  Created on: 28/08/2012
 *      Author: agustin
 *
 */

#ifndef ALPHA_H_
#define ALPHA_H_

#include <OgreTextureUnitState.h>
#include "OverlayEffect.h"

namespace OvEff {

class Alpha : public OverlayEffect {
public:
	/* Transition types */
	enum Type {
		FADE_IN,	// Appear out of thin air
		FADE_OUT	// Dissappear
	};

public:
	inline Alpha();
	inline ~Alpha();

	/**
	 * @brief	Set/Get the time the transition lasts
	 * @param	t	Transition time
	 */
	inline void setTime(float t);
	inline float getTime(void) const;

	/**
	 * @brief	Set/Get the effect type
	 * @param	t	Transition type (i.e. FADE_IN or FADE_OUT)
	 */
	inline void setType(Type t);
	inline Type getType(void) const;

	/**
	 * @brief	Set 'e' as the element to fade in/out.
	 * @param	e	The element that we want to "fade"
	 */
	void configure(Ogre::OverlayElement *e);
	/**
	 * Optionally pass fade parameters to configure().
	 * @param	e		The element that we want to "fade"
	 * @param	time	Transition time
	 * @param	t		Transition type
	 */
	void configure(Ogre::OverlayElement *e, Type t, float time=1.0f);

	/**
	 * @brief	Set the OverlayElement to be used (stub to configure(e))
	 * @param	e	The overlay element
	 */
	void setElement(Ogre::OverlayElement *e);

	/**
	 * @brief	Remove the alpha fading effect from the currently attached
	 * 			OverlayElement. This makes the object fully visible.
	 * @remark	references to the currently attached OverlayElement (if any)
	 * 			are lost.
	 */
	inline void releaseElement(void);

protected:
	/**
	 * Alpha effect start function, initializes the update cycle.
	 */
	virtual void enter(void);

	/**
	 * Alpha effect updating function.
	 */
	virtual void update(void);

	/**
	 * Function called by the OverlayEffectManager when the effect finishes.
	 */
	virtual void exit(void);

private:
	Type					mType;			// FADE_IN / FADE_OUT
	float					mTime;
	Ogre::Real				mAccumTime;
	Ogre::TextureUnitState	*mTexture;		// OverlayElement's texture
};


////////////////////////////////////////////////////////////////////////////////
inline Alpha::Alpha() :
	mType(FADE_IN),
	mTime(1.0f),
	mTexture(0),
	mAccumTime(0)
{
	mElement = 0;
}


////////////////////////////////////////////////////////////////////////////////
inline Alpha::~Alpha() { /* Default destructor suffices. */ }

////////////////////////////////////////////////////////////////////////////////
inline void  Alpha::setTime(float t) { mTime = t; }
inline float Alpha::getTime(void) const { return mTime; }

////////////////////////////////////////////////////////////////////////////////
inline void Alpha::setType(Alpha::Type t) { mType = t; }
inline Alpha::Type Alpha::getType(void) const { return mType; }

////////////////////////////////////////////////////////////////////////////////
inline void Alpha::releaseElement()
{
	if (mTexture) {
		// Remove fading and set internal textue to NULL
		mTexture->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
								Ogre::LBS_TEXTURE, 1.0);
		mTexture = 0;
		mElement = 0;
	}
}

}

#endif /* ALPHA_H_ */
