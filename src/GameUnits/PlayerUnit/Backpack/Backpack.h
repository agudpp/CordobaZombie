/*
 * Backpack.h
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#ifndef BACKPACK_H_
#define BACKPACK_H_

#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreString.h>

#include <vector>

#include "tinyxml.h"
#include "BackpackDefines.h"
#include "BackpackItem.h"


class TinyXmlElement;

#define BACKPACK_OVERLAY_NAME				"BackpackOverlay"
#define BACKPACK_OVERLAY_CONTAINTER_NAME	"BackpackOverlayCont"
#define BACKPACK_ITEMS_OVERLAY				"BackpackItemsOverlay"


class Backpack
{
public:

	typedef std::vector<BackpackItem *>		ItemVec;

public:
	Backpack();
	~Backpack();

	/**
	 * Add a BackpackItem
	 * @param bi	The backpack item to add
	 * @note This class WILL handle the memory
	 */
	void addBackpackItem(BackpackItem *bi);

	/**
	 * Remove a backPackItem
	 * It will remove the memory of the BackpackItem too
	 */
	void removeBackpackItem(BackpackItem *bi);
	void removeBackpackItemUserDef(void *bi);

	/**
	 * Check if the backpack have some item
	 */
	bool hasBackpackItem(BackpackItem *bi) const;

	/**
	 * Check if we have the backPack item associated with a userDef type
	 */
	BackpackItem *hasBackpackItemUserDef(void *ud);

	/**
	 * Show/Hide the backpack items
	 */
	void show(void);
	void hide(void);

	/**
	 * Show/Hide the HUD Backpack element (this is the Backpack)
	 */
	void showBackpack(void);
	void hideBackpack(void);

	/**
	 * Update the elements of the backpack (if some change occur, for example,
	 * if we had fired?)
	 */
	void update(void);

private:
	/**
	 * Load backpack Overlay element (to retrieve the position and the size)
	 */
	void loadBackpackOverlayElement(void);

	/**
	 * Ugly way to be available to show the objects later
	 */
	void showAndHideObjects(void);

	/**
	 * Remove an element (item)
	 */
	inline void removeItem(int index);


private:
	ItemVec					mItems;

	static Ogre::OverlayContainer	*mBackpack;
	static int						mCountReference;
};



////////////////////////////////////////////////////////////////////////////////
inline void Backpack::removeItem(int index)
{
	ASSERT(index >= 0 && index < mItems.size());

	BackpackItem *bi = mItems[index];
	bi->hide();
	mItems[index] = mItems.back();
	mItems.pop_back();
	delete bi;
}

#endif /* BACKPACK_H_ */
