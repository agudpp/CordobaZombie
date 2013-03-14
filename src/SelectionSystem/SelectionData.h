/*
 * SelectionData.h
 *
 *  Created on: 03/12/2012
 *      Author: agustin
 */

#ifndef SELECTIONDATA_H_
#define SELECTIONDATA_H_

#include <vector>
#include <limits>

namespace selection {

// Forward declaration
//
class SelectableObject;


struct SelectionData {
    SelectionData(const std::vector<SelectableObject *> &sel,
                  const std::vector<SelectableObject *> &newSel,
                  const std::vector<SelectableObject *> &unsel) :

        selected(sel)
    ,   lastSelection(newSel)
    ,   unselected(unsel)
    {
    }

    const std::vector<SelectableObject *> &selected;
    const std::vector<SelectableObject *> &lastSelection;
    const std::vector<SelectableObject *> &unselected;
};


// Aditional info used by the SelectableObjects
//
struct Info {
    // The picture index used by the HUD to show the PassportPicture
    // If we have not a index this value will be -1
    unsigned short passportPicture;


    Info() :
        passportPicture(std::numeric_limits<unsigned short>::max())
    {}
};

}

#endif /* SELECTIONDATA_H_ */
