/*
 * EffectsManager.h
 *
 *  Created on: august 7, 2012
 *  Author: agustin
 */

#ifndef EFFECTSMANAGER_H_
#define EFFECTSMANAGER_H_

#include <vector>


class Effect;

class EffectsManager {
public:
	EffectsManager();
	~EffectsManager();

	/**
	 * Add new effect to the EffectManager (to be updated in every frame)
	 * @param	e	The effect to be added (if the effect already exists we do
	 * 				nothing
	 */
	void addEffect(Effect *e);

	/**
	 * Remove effect from the EffectManager
	 * @param	e	The effect to be removed, if the effect is not in the
	 * 				EffectManager then we don't do anything
	 * @note	This class will NOT handle the memory
	 */
	void removeEffect(Effect *e);
	void removeAll(void);

	/**
	 * Check if we have an effect in the EffectManager
	 * @param	e	The effect that we want to check
	 * @return	true if the effect already exists, false otherwise
	 */
	bool hasEffect(Effect *e);

	/**
	 * Update all the effects
	 */
	void update(void);

private:
	std::vector<Effect *>		mEffects;

};

#endif /* EFFECTSMANAGER_H_ */
