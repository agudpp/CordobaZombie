/*
 * HudElement.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef HUDELEMENT_H_
#define HUDELEMENT_H_



// forward declarations
//
namespace selection {
class SelectionManager;
class SelectionData;
}

namespace hud {

class HudElement
{
public:
    HudElement();
    virtual
    ~HudElement();

    /**
     * Load/Unload all the resources of the HudElement
     */
    virtual void
    load(void) = 0;
    virtual void
    unload(void) = 0;

    /**
     * Hide/Show the element
     */
    virtual void
    show(void) = 0;
    virtual void
    hide(void) = 0;

    /**
     * @brief Hihglight the element
     * @param highlight If true then the element should highlight, if false
     *                  it turn off the highlight effect
     */
    virtual void
    highlight(bool highlight) = 0;

    /**
     * @brief Method called every time the selection has changed
     * @param selData   The new selectionData (the current selection)
     */
    virtual void
    selectionChanged(const selection::SelectionData &selData) = 0;
};

}
#endif /* HUDELEMENT_H_ */
