/*
 * UnitWorldContext.h
 *
 *  Created on: 03/03/2012
 *      Author: agustin
 */

#ifndef UNITWORLDCONTEXT_H_
#define UNITWORLDCONTEXT_H_



class UnitWorldContext
{
public:
	UnitWorldContext() {};
	virtual ~UnitWorldContext() {};

	/**
	 * Update the world context
	 */
	virtual void update(void) = 0;

};

#endif /* UNITWORLDCONTEXT_H_ */
