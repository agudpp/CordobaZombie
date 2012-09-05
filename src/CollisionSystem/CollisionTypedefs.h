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
	COL_FLAG_UNIT_PLAYER	= 0x00001,
	COL_FLAG_UNIT_ZOMBIE	= 0x00002,
	COL_FLAG_UNIT_CIVIL		= 0x00004,
	COL_FLAG_WORLD_OBJECT	= 0x00008,
	COL_FLAG_WALL			= 0x00010,
	COL_FLAG_COLLECTABLE_OBJ= 0x00020,
	/*
	wander             = 0x00010,
	cohesion           = 0x00020,
	separation         = 0x00040,
	allignment         = 0x00080,
	obstacle_avoidance = 0x00100,
	wall_avoidance     = 0x00200,
	follow_path        = 0x00400,
	pursuit            = 0x00800,
	evade              = 0x01000,
	interpose          = 0x02000,
	hide               = 0x04000,
	flock              = 0x08000,
	offset_pursuit     = 0x10000,*/
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
};



#endif /* COLLISIONTYPEDEFS_H_ */
