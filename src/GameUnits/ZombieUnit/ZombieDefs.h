/*
 * ZombieDefs.h
 *
 *  Created on: 22/02/2012
 *      Author: agustin
 */

#ifndef ZOMBIEDEFS_H_
#define ZOMBIEDEFS_H_

#include "GameUnitDefines.h"

// DISTANCES DEFINITIONS
#define SOUND_CLOSE_ENOUGH 20.0f
#define SOUND_CLOSE_ENOUGH_SQRDIST SOUND_CLOSE_ENOUGH*SOUND_CLOSE_ENOUGH
#define NEAR_ZOMBIES 100.0f
#define NEAR_ZOMBIES_SQRDIST NEAR_ZOMBIES*NEAR_ZOMBIES

// RANDOM CHOICE PROBABILITIES (PERCENTAGES)
#define IDLE_CHANCE				70  // When idle, chance to play idle animation
#define GRUNT_CHANCE			30  // Chance to emit a grunting sound
#define MOB_GRUNT_CHANCE		15  // Chance to emit group grunts when attacking `en masse`
#define ATTACK_THUD_CHANCE		80  // Chance to emit thud when attacking


// ANIMATIONS OF THE ZOMBIE
#define ZOMB_ANIM_IDLE		"quieto"
#define ZOMB_ANIM_IDLE_2	"quieto2"
#define ZOMB_ANIM_WALK		"camina"
#define ZOMB_ANIM_WALK_2	"camina2"
#define ZOMB_ANIM_RUN		"corre"
#define ZOMB_ANIM_ATTACK	"pega"
#define ZOMB_ANIM_ATTACK_2	"pega2"
#define ZOMB_ANIM_DIE_L		"muereizq"	// fall to the left
#define ZOMB_ANIM_DIE_R		"muereder"
#define ZOMB_ANIM_DIE_B		"muereatr"
#define ZOMB_ANIM_DIE_F		"muereade"
#define ZOMB_ANIM_HIT_F		"golpeade"	// when is been hit from the front
#define ZOMB_ANIM_HIT_B		"golpeatr"	// when is been hit from the back
#define ZOMB_ANIM_SPREAD	"mira"
#define ZOMB_ANIM_SPREAD_2	"mira2"
// TODO: ver si agregamos convulsiones
#define ANIM_ATTACK_COUNT 2
#define ANIM_IDLE_COUNT 2
#define ANIM_WALK_COUNT 2
#define ANIM_RUN_COUNT 1
#define ANIM_SPREAD_COUNT 2


// When zombies advise other that something happen we will pass this structure
// (something like some zombie hear a shoot, or see a human, or anything)
class ZombieUnit;
class GameUnit;

enum {
	ZOMB_FEEL_SEEN,		// when the zombie see the human
	ZOMB_FEEL_HEAR,		// when listen the human
	ZOMB_FEEL_SMELL,
};

typedef struct {
	ZombieUnit	*emitter;
	GameUnit	*target;
	int			feeling;	// XD
} ZombieFeeling;


// Enum used for the Billboards
// Define the Billboards for the players when selected/mouse over
#define ZOMBIE_BB_SEL_NAME			"Zombies/SelectionBb"
#define ZOMBIE_BB_SEL_NUM			2
enum {
	ZOMBIE_BB_MOUSE_OVER,
	ZOMBIE_BB_SELECTION,
};




#endif /* ZOMBIEDEFS_H_ */
