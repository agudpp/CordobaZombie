/*
 * PlayerUnitManager.h
 *
 *  Created on: 13/08/2012
 *      Author: agustin
 *
 */

#ifndef PLAYERUNITMANAGER_H_
#define PLAYERUNITMANAGER_H_

class PlayerUnit;

class PlayerUnitManager {
public:
	enum PlayerID {
		PLAYER_TULIAN,
		PLAYER_CORAL,
		PLAYER_CARDENAL,
		NUM_PLAYERS,
	};
public:

	static inline PlayerUnitManager &getInstance(void)
	{
		static PlayerUnitManager instance;
		return instance;
	}

	/**
	 * Add a player
	 */
	inline void addPlayer(PlayerID id, PlayerUnit *unit);

	/**
	 * Get Player
	 */
	inline PlayerUnit *getPlayer(PlayerID id);

private:
	PlayerUnitManager() : mPlayers({0}) {};
	~PlayerUnitManager(){};

private:
	PlayerUnit	*mPlayers[NUM_PLAYERS];
};

/**
 * Add a player
 */
inline void PlayerUnitManager::addPlayer(PlayerID id, PlayerUnit *unit)
{
	mPlayers[id] = unit;
}

/**
 * Get Player
 */
inline PlayerUnit *PlayerUnitManager::getPlayer(PlayerID id)
{
	return mPlayers[id];
}


#endif /* PLAYERUNITMANAGER_H_ */
