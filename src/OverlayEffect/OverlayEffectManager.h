/*
 * OverlayEffectManager.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef OVERLAYEFFECTMANAGER_H_
#define OVERLAYEFFECTMANAGER_H_

#include <vector>

#include "OverlayEffect.h"
#include "GlobalObjects.h"
#include "DebugUtil.h"

namespace OvEff {

class OverlayEffectManager {
public:

	/**
	 * Add a new effect to the manager.
	 * @param	e	The effect to be added (it shouldn't exists in the manager)
	 */
	inline void add(OverlayEffect *e);

	/**
	 * Remove an effect from the manager
	 * @param	e	The effect to remove (it should exists in the manager)
	 */
	inline void remove(OverlayEffect *e);

	/**
	 * Verify if exists an effect in the manager
	 * @param	e	The effect to be checked
	 * @return	true if the effect exists, false otherwise
	 */
	inline bool hasEffect(OverlayEffect *e) const;

	/**
	 * Update all the effects
	 */
	inline void update(void);

private:
	std::vector<OverlayEffect *>	mEffects;
};


////////////////////////////////////////////////////////////////////////////////
inline void OverlayEffectManager::add(OverlayEffect *e)
{
	ASSERT(!hasEffect(e));
	e->begin();
	e->setIndex(mEffects.size());
	mEffects.push_back(e);

}

////////////////////////////////////////////////////////////////////////////////
inline void OverlayEffectManager::remove(OverlayEffect *e)
{
	ASSERT(hasEffect(e));
	const int s = mEffects.size()-1;
	mEffects[s]->setIndex(e->getIndex());
	mEffects[e->getIndex()] = mEffects[s];
	mEffects.pop_back();

	// advise the effect
	e->end();
}

////////////////////////////////////////////////////////////////////////////////
inline bool OverlayEffectManager::hasEffect(OverlayEffect *e) const
{
	ASSERT(e);
	if(e->getIndex() < mEffects.size() && mEffects[e->getIndex()] == e)
		return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
inline void OverlayEffectManager::update(void)
{
	const int size = mEffects.size();
	for(int i = 0; i < size; ++i){
		mEffects[i]->update();
	}
}

}

#endif /* OVERLAYEFFECTMANAGER_H_ */
