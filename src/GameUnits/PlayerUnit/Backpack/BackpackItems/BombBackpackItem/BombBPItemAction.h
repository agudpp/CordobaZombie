/*
 * BombBPItemAction.h
 *
 *  Created on: 01/08/2012
 *      Author: agustin
 *
 */

#ifndef BOMBBPITEMACTION_H_
#define BOMBBPITEMACTION_H_


class PlayerUnit;
class Bomb;

// Define the interface of objects that will encapsulate the functionality
// associated with the bombs
class BombBPItemAction
{
public:
	virtual void operator()(PlayerUnit *owner, Bomb *bomb) = 0;
};

#endif /* BOMBBPITEMACTION_H_ */
