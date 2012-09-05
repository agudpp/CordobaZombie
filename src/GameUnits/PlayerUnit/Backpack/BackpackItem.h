/*
 * BackpackItem.h
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#ifndef BACKPACKITEM_H_
#define BACKPACKITEM_H_

#include <OgreOverlayElement.h>
#include <OgreString.h>

#include "DebugUtil.h"
#include "BackpackDefines.h"
#include "BpItemAdditionalInfo.h"
#include "MenuButton.h"

class PlayerUnit;

/**
 * TODO: La idea es que cada objeto (bomb, gun, item) tenga asociado un BackpackItem
 * y que este mismo tenga asociado a su vez el objeto en si, con un inmundo
 * typedef vamos a tener que diferenciar el tipo y castearlo nuevamente.
 * Este backpack solo sirve para la interaccion del jugador (visual) con los
 * elementos del player en si.
 */

class BackpackItem : public MenuButton
{
public:
	BackpackItem(int section = BackpackDef::SECTION_NONE);
	virtual ~BackpackItem();

	/**
	 * Returns the item section
	 */
	inline void setSection(int s);
	inline int getSection(void) const;

	/**
	 * Show / Hide the item
	 */
	inline void show(void);
	inline void hide(void);

	/**
	 * Returns the info if it have.
	 */
	inline void setAdditionalInfo(const BpItemAdditionalInfo &info);
	inline const BpItemAdditionalInfo &getAdditionalInfo(void) const;

	/**
	 * Ugly and nasty ... Used to get again the object itself
	 */
	inline void setUserDef(void *ud);
	inline void *getUserDef(void);

	// testing functionallity...
	template<typename T>
	inline void getUserDef(T *&u) {u = static_cast<T>(mUserDef);}

	/**
	 * Set / Get the owner (PlayerUnit)
	 */
	inline PlayerUnit *getOwner(void);
	inline void setOwner(PlayerUnit *);


	/**
	 * This function is called when the visual item was positioned in the
	 * backpack.
	 */
	virtual void updated(void) = 0;

protected:

	/**
	 * When the left mouse button is pressed what we will do is show the
	 * AdditionalInfo of this BackpackItem.
	 */
	virtual void rightPressed(void);

	////////////////////////////////////////////////////////////////////////////
	////						CALLBACKS TO MPLEMENT						////
	////////////////////////////////////////////////////////////////////////////

	/**
	 * When the button is left Pressed
	 */
	virtual void leftPressed(void) = 0;

protected:
	void					*mUserDef;
	PlayerUnit				*mOwner;

private:
	int						mSection;
	BpItemAdditionalInfo	mAdditionalInfo;
};



////////////////////////////////////////////////////////////////////////////////
inline void BackpackItem::setSection(int s)
{
	mSection = s;
}
inline int BackpackItem::getSection(void) const
{
	return mSection;
}

////////////////////////////////////////////////////////////////////////////////
inline void BackpackItem::show(void)
{
	setVisible(true);
	setEnable(true);
}
////////////////////////////////////////////////////////////////////////////////
inline void BackpackItem::hide(void)
{
	setVisible(false);
	setEnable(false);
}

////////////////////////////////////////////////////////////////////////////////
inline void BackpackItem::setAdditionalInfo(const BpItemAdditionalInfo &info)
{
	mAdditionalInfo = info;
}

////////////////////////////////////////////////////////////////////////////////
inline const BpItemAdditionalInfo &BackpackItem::getAdditionalInfo(void) const
{
	return mAdditionalInfo;
}


////////////////////////////////////////////////////////////////////////////////
inline void BackpackItem::setUserDef(void *ud)
{
	mUserDef = ud;
}

////////////////////////////////////////////////////////////////////////////////
inline void *BackpackItem::getUserDef(void)
{
	return mUserDef;
}


////////////////////////////////////////////////////////////////////////////////
inline PlayerUnit *BackpackItem::getOwner(void)
{
	return mOwner;
}
inline void BackpackItem::setOwner(PlayerUnit *owner)
{
	mOwner = owner;
}


#endif /* BACKPACKITEM_H_ */
