/*
 * BackpackElement.h
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#ifndef BACKPACKELEMENT_H_
#define BACKPACKELEMENT_H_

#include <boost/shared_ptr.hpp>

#include <GameUnits/PlayerUnit/Backpack/BackpackItem.h>
#include <MenuSystem/GUI/Button/CallbackMenuButton.h>
#include <debug/DebugUtil.h>


// Forward
//
class PlayerUnit;

namespace hud {

class BackpackElement
{
public:
    BackpackElement(BackpackItem* item, CallbackMenuButtonPtr& button);
    virtual
    ~BackpackElement() {};

    /**
     * @brief Returns the backpackitem ptr
     */
    inline const BackpackItem*
    getBackpackItemPtr(void) const;
    inline BackpackItem*
    getBackpackItemPtr(void);

    /**
     * @brief Get/Set the player unit associated to this item
     */
    inline PlayerUnit*
    playerUnit(void);
    inline void
    setPlayerUnit(PlayerUnit*);

    /**
     * @brief hide / show this element
     */
    inline void
    hide(void);
    inline void
    show(void);

    /**
     * @brief Check if this element is visible or not
     */
    inline bool
    isVisible(void) const;

    /**
     * @brief Check if the element is dirty or not
     */
    inline bool
    isDirty(void) const;

    /**
     * @brief Callback for when the button is pressed
     */
    virtual void
    buttonPressed(CallbackMenuButton::ButtonID) = 0;

    /**
     * @brief Virtual method that will be called every time we need to update
     *        the element (when is dirty for example).
     *        In this method we should put all the logic for update the visual
     *        information
     */
    virtual void
    update(void) = 0;

    /**
     * @brief Method called every time the selection has changed
     * @param selData   The new selectionData (the current selection)
     */
    virtual void
    selectionChanged(const selection::SelectionData &selData) = 0;

protected:
    bool mIsDirty;
    PlayerUnit *mPlayer;
private:
    BackpackItem* mItem;
    CallbackMenuButtonPtr mButton;

};

// typedef for shared ptr
//
typedef boost::shared_ptr<BackpackElement> BackpackElementPtr;


// Inline impl
//
BackpackElement::BackpackElement(BackpackItem* item, CallbackMenuButtonPtr& button) :
    mIsDirty(true)
,   mPlayer(0)
,   mItem(item)
,   mButton(button)
{}

inline const BackpackItem*
BackpackElement::getBackpackItemPtr(void) const
{
    return mItem;
}
inline BackpackItem*
BackpackElement::getBackpackItemPtr(void)
{
    return mItem;
}

inline PlayerUnit*
BackpackElement::playerUnit(void)
{
    return mPlayer;
}
inline void
BackpackElement::setPlayerUnit(PlayerUnit* pu)
{
    mPlayer = pu;
}

inline void
BackpackElement::hide(void)
{
    ASSERT(mButton.get());
    mButton->setEnable(false);
}
inline void
BackpackElement::show(void)
{
    ASSERT(mButton.get());
    mButton->setEnable(true);
}

inline bool
BackpackElement::isVisible(void) const
{
    ASSERT(mButton.get());
    return mButton->isActive();
}

inline bool
BackpackElement::isDirty(void) const
{
    return mIsDirty;
}

} /* namespace hud */
#endif /* BACKPACKELEMENT_H_ */
