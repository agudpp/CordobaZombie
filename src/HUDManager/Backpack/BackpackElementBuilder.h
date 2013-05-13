/*
 * BackpackElementBuilder.h
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#ifndef BACKPACKELEMENTBUILDER_H_
#define BACKPACKELEMENTBUILDER_H_

#include "BackpackElement.h"


// forward
//
class BackpackItemPtr;
class PlayerUnit;


namespace hud {

class BackpackElementBuilder
{
public:
    BackpackElementBuilder();
    virtual
    ~BackpackElementBuilder();

    /**
     * @brief Build a BackpackElementPtr from a BackpackItemPtr.
     * @param item      The item used to construct the BackpackElement
     * @param player    The associated PlayerUnit to this element
     * @returns The BackpackElementPtr, this class will not handle the memory
     *          of the element anymore
     */
    BackpackElementPtr
    buildElement(const BackpackItemPtr& item, PlayerUnit* player);
};

} /* namespace hud */
#endif /* BACKPACKELEMENTBUILDER_H_ */
