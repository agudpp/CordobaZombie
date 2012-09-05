/*
 * ZombieFSMBuilder.h
 *
 *  Created on: 28/02/2012
 *      Author: agustin
 *
 */

#ifndef ZOMBIEFSMBUILDER_H_
#define ZOMBIEFSMBUILDER_H_

#include "ZombieDefs.h"
#include "SMTransitionTable.h"
#include "ZombieUnit.h"


class ZombieFSMBuilder {
public:

	/**
	 * Builds the state machine of the zombie unit
	 */
	static ZombieSMTTable *build(void);

};

#endif /* ZOMBIEFSMBUILDER_H_ */
