/*
 * GameUnitDefines.h
 *
 *  Created on: 09/02/2012
 *      Author: agustin
 *
 */

#ifndef GAMEUNITDEFINES_H_
#define GAMEUNITDEFINES_H_

#include "CommonMath.h"
#include "CollisionTypedefs.h"

typedef int		live_t;


#define ZOMBIE_MAX_VISIBILITY_DIST		300.0f
#define PLAYER_MAX_VISIBILITY_DIST		300.0f
#define CIVIL_MAX_VISIBILITY_DIST		300.0f
#define ZOMBIE_MAX_VISIBILITY_SQRDIST	ZOMBIE_MAX_VISIBILITY_DIST*ZOMBIE_MAX_VISIBILITY_DIST
#define PLAYER_MAX_VISIBILITY_SQRDIST	PLAYER_MAX_VISIBILITY_DIST*PLAYER_MAX_VISIBILITY_DIST
#define CIVIL_MAX_VISIBILITY_SQRDIST	CIVIL_MAX_VISIBILITY_DIST*CIVIL_MAX_VISIBILITY_DIST


// Define a Hit_t used to determine the hit object
class GameUnit;

typedef struct {
	sm::Vector2		hitDir;
	float			power;
	GameUnit		*shooter;

	enum {
		FRONT = 0,
		BACK,
		LEFT,
		RIGHT
	};
} Hit_t;


// Define in a simple way what the unit is doing
typedef struct {
	sm::Vector2			goingTo;	// this probably will not be used (depend
									// of the event)
	GameUnit 			*target;
	int					event;
} UnitIAState;


#endif /* GAMEUNITDEFINES_H_ */
