/*
 * PlayerUnitManager.h
 *
 *  Created on: 13/08/2012
 *      Author: agustin
 *
 */

#ifndef PLAYERUNITMANAGER_H_
#define PLAYERUNITMANAGER_H_

#include "PlayerDefs.h"

class PlayerUnit;

class PlayerUnitManager {
public:
	enum PlayerID {
		PLAYER_TULIAN = ::PlayerID::PLAYER_TULIAN,
		PLAYER_CORAL = ::PlayerID::PLAYER_CORAL,
		PLAYER_CARDENAL = ::PlayerID::PLAYER_CARDENAL,
		NUM_PLAYERS = ::PlayerID::NUM_PLAYERS,
	};
public:

	static inline PlayerUnitManager &getInstance(void)
	{
		static PlayerUnitManager instance;
		return instance;
	}

	/**
	 * @brief Add a player to be tracked there (memory track)
	 * @param  unit  The unit (Player) to be tracked.
	 */
	inline void
	addPlayer(PlayerUnitPtr &unit);

	/**
	 * @brief Get Player directly.
	 */
	inline PlayerUnit *
	getPlayer(PlayerID id) const;

	inline PlayerUnitPtr
	getPlayerPtr(PlayerID id) const;

	/**
	 * @brief Clear all the players
	 */
	inline void
	clear(void);

private:
	PlayerUnitManager(){};
	~PlayerUnitManager(){};

private:
	PlayerUnitPtr	mPlayers[NUM_PLAYERS];
};




inline void
PlayerUnitManager::addPlayer(PlayerUnitPtr &unit)
{
    ASSERT(unit.get());
    mPlayers[unit->playerID()] = unit;
}


inline PlayerUnit *
PlayerUnitManager::getPlayer(PlayerID id) const
{
    return mPlayers[id].get();
}
inline PlayerUnitPtr
PlayerUnitManager::getPlayerPtr(PlayerID id) const
{
    return mPlayers[id];
}

inline void
PlayerUnitManager::clear(void)
{
    for (size_t i = 0; i < NUM_PLAYERS; ++i) {
        mPlayers[i].reset();
    }
}


#endif /* PLAYERUNITMANAGER_H_ */
