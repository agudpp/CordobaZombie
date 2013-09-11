/*
 * FSMState.h
 *
 *  Created on: Sep 7, 2013
 *      Author: agustin
 */

#ifndef FSMSTATE_H_
#define FSMSTATE_H_

namespace cz {

// @brief This class will represent the generic states of the FSM, this states
//        contains 3 methods (enter, update, exit) and all of them recieve the same
//        type of argument
//        They also should be identified by an unique id.

template <typename ArgType, typename IDType = unsigned int>
struct FSMState
{
public:
    FSMState() {};
    virtual
    ~FSMState() {};

    // @brief Set / Get the ID for this state
    // @param id    The id to be set
    //
    inline void
    setID(const IDType& id) {mID = id;}
    inline const IDType&
    ID(void) const {return mID;}


    // @brief Method called when when enter to this state
    //
    virtual void
    enter(ArgType) = 0;

    // @brief Method called when when update this state
    //
    virtual void
    update(ArgType) = 0;

    // @brief Method called when when exit this state
    //
    virtual void
    exit(ArgType) = 0;

protected:
    IDType mID;
};

} /* namespace cz */
#endif /* FSMSTATE_H_ */
