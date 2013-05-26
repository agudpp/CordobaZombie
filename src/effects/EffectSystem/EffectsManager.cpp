/*
 * EffectsManager.cpp
 *
 *  Created on: august 7, 2012
 *      Author: agustin
 */

#include "EffectsManager.h"
#include "Effect.h"
#include "DebugUtil.h"

EffectsManager::EffectsManager()
{

}
EffectsManager::~EffectsManager()
{

}

////////////////////////////////////////////////////////////////////////////////
void EffectsManager::addEffect(Effect *e)
{
	ASSERT(e);
	ASSERT(!hasEffect(e));

	e->setIndex(mEffects.size());
	mEffects.push_back(e);
}

////////////////////////////////////////////////////////////////////////////////
void EffectsManager::removeEffect(Effect *e)
{
	ASSERT(e);
	ASSERT(hasEffect(e));
	ASSERT(e->getIndex() < mEffects.size());
	ASSERT(mEffects[e->getIndex()] == e);

	Effect *last = mEffects.back();
	mEffects[e->getIndex()] = last;
	last->setIndex(e->getIndex());

	mEffects.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
void EffectsManager::removeAll(void)
{
	mEffects.clear();
}

////////////////////////////////////////////////////////////////////////////////
bool EffectsManager::hasEffect(Effect *e)
{
	ASSERT(e);
	return (e->getIndex() < mEffects.size()) && (mEffects[e->getIndex()] == e);
}

////////////////////////////////////////////////////////////////////////////////
void EffectsManager::update(void)
{
	int size = mEffects.size();
	for(int i = 0; i < size; ++i){
		mEffects[i]->update();
	}
}
