/*
 * BackpackRep.h
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#ifndef BACKPACKREP_H_
#define BACKPACKREP_H_

#include <HUDManager/HudElement.h>

namespace hud {

class BackpackRep : public HudElement
{
public:
    BackpackRep();
    virtual
    ~BackpackRep();

    ////////////////////////////////////////////////////////////////////////////
    //                          HudElement Methods                            //
    ////////////////////////////////////////////////////////////////////////////
    //

    /**
     * Load/Unload all the resources of the HudElement
     */
    virtual void load(void);
    virtual void unload(void);

    /**
     * Hide/Show the element
     */
    virtual void show(void);
    virtual void hide(void);

    /**
     * @brief Hihglight the element
     * @param highlight If true then the element should highlight, if false
     *                  it turn off the highlight effect
     */
    virtual void highlight(bool highlight);


    ////////////////////////////////////////////////////////////////////////////
    //                   BackpackRepresentation methods                       //
    ////////////////////////////////////////////////////////////////////////////
    //


};

} /* namespace hud */
#endif /* BACKPACKREP_H_ */
