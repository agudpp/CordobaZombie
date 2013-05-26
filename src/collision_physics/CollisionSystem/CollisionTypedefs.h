/*
 * CollisionTypedefs.h
 *
 *  Created on: 16/02/2012
 *      Author: agustin
 *
 */

#ifndef COLLISIONTYPEDEFS_H_
#define COLLISIONTYPEDEFS_H_


#include "CommonMath.h"

// mask type
typedef int		mask_t;
typedef short	objectType_t;

typedef float 	cScalar;







/**
 * Here we will define all the possible collision Masks and types
 */

// The flags used for the game units
enum {
	COL_FLAG_UNIT_PLAYER	= 1,
	COL_FLAG_UNIT_ZOMBIE	= (1 << 1),
	COL_FLAG_UNIT_CIVIL		= (1 << 2),
	COL_FLAG_WORLD_OBJECT	= (1 << 3),
	COL_FLAG_WALL			= (1 << 4),
	COL_FLAG_COLLECTABLE_OBJ= (1 << 5),
	COL_FLAG_LEVEL_INFO_OBJ = (1 << 6),
};

/**
 * Define the mask used to get the objects that can be affected
 */
enum{
	// Mask used to get object that are affected and not affected by a bomb
	BOMB_AFFECTABLE_MASK		= COL_FLAG_UNIT_PLAYER | COL_FLAG_UNIT_ZOMBIE,
	BOMB_NOT_AFFECT_MASK		= ~BOMB_AFFECTABLE_MASK,

	// Mask used to get all the objects that can receive a "shoot". This are
	// all except the player itself
	SHOOT_AFFECTABLE_MASK		= COL_FLAG_UNIT_PLAYER ^ ~0,

	//Vision obstacles mask
	VISION_OBST_MASK			= COL_FLAG_WALL,

	//Player Enemies
	PLAYER_ENEMIES_MASK			= COL_FLAG_UNIT_ZOMBIE,
};


// TODO: terminar de setear todo esto
// define the GroupFlags here
enum {
	COL_GRFLAG_UNIT_PLAYER = ~0,
	COL_GRFLAG_UNIT_ZOMBIE = ~0,
	COL_GRFLAG_UNIT_CIVIL  = ~0,
	COL_GRFLAG_COLL_OBJECT = 0,     // None
};



#endif /* COLLISIONTYPEDEFS_H_ */
