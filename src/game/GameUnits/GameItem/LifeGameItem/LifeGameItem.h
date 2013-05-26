/*
 * LifeGameItem.h
 *
 *  Created on: 01/08/2012
 *      Author: agustin
 *
 */

#ifndef LIFEGAMEITEM_H_
#define LIFEGAMEITEM_H_

#include "GameItem.h"
#include "GameUnitDefines.h"

class LifeGameItem : public GameItem {
public:
	LifeGameItem();
	virtual ~LifeGameItem();

	/**
	 * Set/Get the amount of life of the LifeItem
	 */
	inline live_t getLife(void) const;
	inline void setLife(live_t l);

	/**
	 * Use the medikit in the same unit
	 */
	virtual void useIt(void);

protected:
	live_t		mLife;
};






////////////////////////////////////////////////////////////////////////////////
inline live_t LifeGameItem::getLife(void) const
{
	return mLife;
}
inline void LifeGameItem::setLife(live_t l)
{
	mLife = l;
}


#endif /* LIFEGAMEITEM_H_ */
