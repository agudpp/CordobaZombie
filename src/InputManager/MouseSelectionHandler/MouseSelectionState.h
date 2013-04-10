/*
 * MouseSelectionState.h
 *
 *  Created on: 16/12/2012
 *      Author: agustin
 */

#ifndef MOUSESELECTIONSTATE_H_
#define MOUSESELECTIONSTATE_H_

namespace input {

class MouseSelectionState {
public:
    MouseSelectionState();
    ~MouseSelectionState();

    /**
     * Returns the associated raycast mask we need to use
     */
    uint32 getMask(void) const;
};

}

#endif /* MOUSESELECTIONSTATE_H_ */
