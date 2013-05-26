/*
 * PlayerWorldContext.h
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#ifndef PLAYERWORLDCONTEXT_H_
#define PLAYERWORLDCONTEXT_H_

#include <vector>


class GameUnit;
class PlayerUnit;



class PlayerWorldContext
{
public:
	typedef std::vector<GameUnit *>	ZombieUnitVec;
	typedef std::vector<PlayerUnit *>	PlayerVector;
public:
	PlayerWorldContext(PlayerUnit *pu);
	virtual ~PlayerWorldContext();

	/**
	 * Update the world context
	 */
	void update(void);

	/**
	 * Get closer zombies to the unit can see
	 */
	inline ZombieUnitVec &getVisibleZombies(void);

	/**
	 * Get all the unit closer to this, all that we can see
	 */
	inline PlayerVector &getVisiblePlayer(void);

private:
	/**
	 * Real Update of zombies/player closer
	 */
	void updateZombies(void);
	void updatePlayers(void);


private:
	enum {
		NONE				=	0,
		ZOMBIE_UPDATED		=	1,
		PLAYER_UPDATED		=	2,
	};


	PlayerUnit 				*mRef;
	ZombieUnitVec			mVisibleZombies;
	PlayerVector			mVisiblePlayers;
	int						mFlags;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
inline PlayerWorldContext::ZombieUnitVec &PlayerWorldContext::getVisibleZombies(void)
{
	if(!(mFlags & ZOMBIE_UPDATED))	{updateZombies();}
	return mVisibleZombies;
}

////////////////////////////////////////////////////////////////////////////////
inline PlayerWorldContext::PlayerVector &PlayerWorldContext::getVisiblePlayer(void)
{
	if(!(mFlags & PLAYER_UPDATED))	{updatePlayers();}
	return mVisiblePlayers;
}



#endif /* PLAYERWORLDCONTEXT_H_ */
