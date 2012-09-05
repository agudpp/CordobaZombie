/*
 * GameItem.h
 *
 *  Created on: 31/07/2012
 *      Author: agustin
 *
 */

#ifndef GAMEITEM_H_
#define GAMEITEM_H_

class GameUnit;

class GameItem {
public:
	GameItem();
	virtual ~GameItem();

	/**
	 * Set/Get the owner of the item
	 */
	inline void setOwner(GameUnit *);
	inline GameUnit *getOwner(void);

	/**
	 * Set/Get the type of the Item
	 */
	inline void setID(unsigned short id);
	inline unsigned short getID(void) const;

	/**
	 * Virtual function to be implemented when the item has to be used.
	 */
	virtual void useIt(void) = 0;


protected:
	GameUnit		*mOwner;
	unsigned short	mID;
};


////////////////////////////////////////////////////////////////////////////////
inline void GameItem::setOwner(GameUnit *o)
{
	mOwner = o;
}
inline GameUnit *GameItem::getOwner(void)
{
	return mOwner;
}
////////////////////////////////////////////////////////////////////////////////
inline void GameItem::setID(unsigned short id)
{
	mID = id;
}
inline unsigned short GameItem::getID(void) const
{
	return mID;
}


#endif /* GAMEITEM_H_ */
