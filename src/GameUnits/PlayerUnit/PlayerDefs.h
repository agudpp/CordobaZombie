/*
 * PlayerDefs.h
 *
 *  Created on: 02/03/2012
 *      Author: agustin
 */

#ifndef PLAYERDEFS_H_
#define PLAYERDEFS_H_

#include "GameUnitDefines.h"

// ANIMATIONS OF THE ZOMBIE
#define PLAYER_ANIM_IDLE			"quieto"
#define PLAYER_ANIM_WALK			"camina"
#define PLAYER_ANIM_RUN				"corre"
#define PLAYER_ANIM_ATTACK_B		"pega_cuacu"
#define PLAYER_ANIM_ATTACK_S2		"pega_corta2"
#define PLAYER_ANIM_ATTACK_S		"pega_corta"
#define PLAYER_ANIM_RELOAD_S		"recarga_corta"
#define PLAYER_ANIM_ATTACK_L		"pega_long"
#define PLAYER_ANIM_ATTACK_L2		"pega_long2"
#define PLAYER_ANIM_RELOAD_L		"recarga_long"
#define PLAYER_ANIM_ATTACK_T		"pega_gra"
#define PLAYER_ANIM_DIE_L			"muereizq"	// fall to the left
#define PLAYER_ANIM_DIE_R			"muereder"
#define PLAYER_ANIM_DIE_B			"muereatr"
#define PLAYER_ANIM_DIE_F			"muereade"
#define PLAYER_ANIM_HIT_F			"golpeade"	// when is been hit from the front
#define PLAYER_ANIM_HIT_B			"golpeatr"	// when is been hit from the back
#define PLAYER_ANIM_PICK_OBJECT		"recoge"
#define	PLAYER_ANIM_PLANT_BOMB		"plantar_bomba"
#define PLAYER_ANIM_GAB_MOV_OBJ		"agarra_obj"	// anims used to move the box or
#define PLAYER_ANIM_PUSH_MOV_OBJ	"empuja_obj"	// movable objects by the player
#define PLAYER_ANIM_PULL_MOV_OBJ	"tira_obj"


#define PLAYER_HAND_BONE			"palma"



// THE IA EVENTs associated to the player
enum {
	IAEP_NONE = 0,
	IAEP_ENGAGING_UNIT,
	IAEP_PLANTING_BOMB,
	IAEP_IDLE,
	IAEP_MOVING
};


// Enum used for the Billboards
// Define the Billboards for the players when selected/mouse over
#define PLAYER_BB_SEL_NAME			"Players/SelectionBb"
#define PLAYER_BB_SEL_NUM			2
enum {
	PLAYER_BB_MOUSE_OVER = 0,
	PLAYER_BB_SELECTION,
};


#endif /* PLAYERDEFS_H_ */
