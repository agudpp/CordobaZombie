/*
 * BombBackpackItem.h
 *
 *  Created on: 01/08/2012
 *      Author: agustin
 *
 */

#ifndef BOMBBACKPACKITEM_H_
#define BOMBBACKPACKITEM_H_

#include "BackpackItem.h"
#include "BombBPItemAction.h"


class BombBackpackItem : public BackpackItem {
public:
	BombBackpackItem();
	virtual ~BombBackpackItem();

	/**
	 * Set the action to be executed when the bomb is clicked.
	 * @note	This class WILL NOT handle the memory of the action
	 */
	inline void setAction(BombBPItemAction *a);
	inline BombBPItemAction *getAction(void);

	/**
	 * This function is called when the visual item was positioned in the
	 * backpack.
	 */
	virtual void updated(void);

protected:
	////////////////////////////////////////////////////////////////////////////
	////						CALLBACKS TO MPLEMENT						////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * When the button is left Pressed
	 */
	virtual void leftPressed(void);

private:
	BombBPItemAction	*mAction;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void BombBackpackItem::setAction(BombBPItemAction *a)
{
	mAction = a;
}
inline BombBPItemAction *BombBackpackItem::getAction(void)
{
	return mAction;
}

#endif /* BOMBBACKPACKITEM_H_ */
