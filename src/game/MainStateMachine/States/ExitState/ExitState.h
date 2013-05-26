/*
 * ExitState.h
 *
 *  Created on: 06/10/2012
 *      Author: agustin
 *
 */

#ifndef EXITSTATE_H_
#define EXITSTATE_H_


#include "IMainState.h"


class ExitState : public IMainState{
public:
    ExitState();
    virtual ~ExitState();

    /**
     * Entering the state with additional info
     */
    virtual void enter(const MainMachineInfo &info);

    /**
     * Update the state... This function will be called once.
     * @param   info    The structure used to pass throw the states. If we want
     *                  to fill it with some information then we can do it.
     * @return  event   The event that was happend.
     */
    virtual MainMachineEvent update(MainMachineInfo &info);

    /**
     * Function called when the state is not "the actual" anymore
     */
    virtual void exit(void);
};

#endif /* EXITSTATE_H_ */
