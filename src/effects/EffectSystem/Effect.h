/*
 * Effect.h
 *
 *  Created on: 07/08/2012
 *      Author: agustin
 */

#ifndef EFFECT_H_
#define EFFECT_H_

#include <OgreSceneNode.h>
#include <OgreVector3.h>

#include "QueueContainer.h"


class EffectsManager;
class Effect;

// Define the QueueContainer of the effects
typedef QueueContainer<Effect>		EffectQueue;


class Effect
{
public:
	/**
	 * Creates the effect with the Associated queue. This queue will be used when
	 * this effects stops (to be put in the queue automatically)
	 * @note	The constructor will NOT automatically add this object to the
	 * 			queue.
	 */
	Effect(EffectQueue *q, bool createSceneNode = true);
	virtual ~Effect();

	/**
	 * Set the EffectManager instance to be used by the effects
	 */
	static void setEffectManager(EffectsManager *em);

	/**
	 * Start to reproduce the effect (this will add the effect to the
	 * EffectManager and will call the beforeStart function)
	 * @param	pos		The position where we will start the effect
	 * @param	dir		The direction (if needed) of the effect
	 *
	 * @note	This position and direction are set to the SceneNode.
	 */
	virtual void start(const Ogre::Vector3 &pos,
			const Ogre::Vector3 &dir = Ogre::Vector3::ZERO);

	/**
	 * Stops the effect (if we are already reproducing it). If we are, the
	 * afterStop function will be called.
	 */
	virtual void stop(void);

	/**
	 * The main function, here we have to update the progress of the effect
	 */
	virtual void update(void) = 0;

	/**
	 * Returns the scene node
	 */
	inline Ogre::SceneNode *getSceneNode(void);

	/**
	 * Show/Hide the effect
	 */
	void show(void);
	void hide(void);

protected:
	////////////////////////////////////////////////////////////////////////////
	//					This functions have to be implemented				////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * Function called before we start updating the effect, so we have to do
	 * all the things we need here
	 */
	virtual void beforeUpdate(void) = 0;

	/**
	 * Function called after we have not to update any more the effect (after
	 * the EffectsManager pop us from the Active Effects)
	 */
	virtual void afterStop(void) = 0;

public:
	// TODO: aca estamos teniendo un problema que no nos deja usar el friend
	// No se porque, esto habria que meterlo como "private" y no como "public"
	friend class EffecsManager;

	/**
	 * Functions used by the EffectsManager
	 */
	inline int getIndex(void);
	inline void setIndex(int i);

private:
	Ogre::SceneNode				*mSceneNode;
	int							mIndexPosition;
	EffectQueue					*mQueue;

	static EffectsManager		*mEffectsManager;
};




inline Ogre::SceneNode *Effect::getSceneNode(void)
{
	return mSceneNode;
}

inline int Effect::getIndex(void)
{
	return mIndexPosition;
}
inline void Effect::setIndex(int i)
{
	mIndexPosition = i;
}

#endif /* EFFECT_H_ */
