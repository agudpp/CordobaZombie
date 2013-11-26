/*
 * MainStateBuilder.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef MAINSTATEBUILDER_H_
#define MAINSTATEBUILDER_H_


#include "MainStateInformation.h"
#include "MainStateID.h"

namespace cz {

// forward
//
class IMainState;


class MainStateBuilder
{
public:
    MainStateBuilder();
    ~MainStateBuilder();

    // @brief Configure the builder with all the information we need here for
    //        all the states.
    // TODO

    // @brief Set the Ogre common information to be used by all the States
    // @param ogreData      The ogre Data to be used
    //
    void
    setOgreCommon(const OgreCommon& ogreData);

    // @brief Build a new state from a given ID. This will allocate memory
    //        and the state should be free when it is not used anymore. This
    //        means that the caller of this method is the responsible of the
    //        memory.
    // @param id        The state id we want to build
    // @return the new allocated state | 0 on error.
    //
    IMainState*
    buildState(const MainStateID& id) const;
};

} /* namespace cz */
#endif /* MAINSTATEBUILDER_H_ */
