/*
 * PlayerWorldContext.cpp
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#include "DebugUtil.h"
#include "PlayerWorldContext.h"
#include "PlayerUnit.h"

/**
 * Real Update of zombies/player closer
 */
void PlayerWorldContext::updateZombies(void)
{
	mVisibleZombies.clear();
	PlayerWorldContext::ZombieUnitVec &vec = mRef->getNearbyZombies();
	if(!vec.empty()){
		// we have zombies closer, check one by one if we can see it
		for(int i = vec.size()-1; i >= 0; --i){
			if(mRef->haveLineOfSightTo(vec[i], PLAYER_MAX_VISIBILITY_SQRDIST)){
				mVisibleZombies.push_back(vec[i]);
			}
		}
	}
	mFlags |= ZOMBIE_UPDATED;
}

void PlayerWorldContext::updatePlayers(void)
{
	mVisiblePlayers.clear();
	mRef->getNearbyPlayers(mVisiblePlayers);
	if(!mVisiblePlayers.empty()){
		int size =  mVisiblePlayers.size();
		// we have zombies closer, check one by one if we can see it
		for(int i = 0; i < size; ++i){
				if(!mRef->haveLineOfSightTo(mVisiblePlayers[i],
						PLAYER_MAX_VISIBILITY_SQRDIST)){
					// we have to remove the actual player
					mVisiblePlayers[i] = mVisiblePlayers[mVisiblePlayers.size()-1];
					mVisiblePlayers.pop_back();
					--i;	// check the actual value now
					--size;
			}
		}
	}
	mFlags |= PLAYER_UPDATED;
}



PlayerWorldContext::PlayerWorldContext(PlayerUnit *pu) :
mRef(pu),
mFlags(0)
{
	ASSERT(mRef);
}

PlayerWorldContext::~PlayerWorldContext()
{
}


/**
 * Update the world context
 */
void PlayerWorldContext::update(void)
{
	// work in lazy mode.. what that the user ask for any function
	mFlags = NONE;
}
