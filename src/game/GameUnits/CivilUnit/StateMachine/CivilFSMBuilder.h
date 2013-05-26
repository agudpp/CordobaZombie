/*
 * CivilFSMBuilder.h
 *
 *  Created on: 16/07/2012
 *      Author: agustin
 */

#ifndef CIVILFSMBUILDER_H_
#define CIVILFSMBUILDER_H_

#include "CivilUnit.h"

class CivilFSMBuilder
{
public:

	/**
	 * Builds the state machine of the zombie unit
	 */
	static CivilSMTTable *build(void);
};

#endif /* CIVILFSMBUILDER_H_ */
