/*
 * IMainState.h
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#ifndef IMAINSTATE_H_
#define IMAINSTATE_H_

#include <string>

#include "MainStateMachineDefs.h"

class IMainState
{
public:
	IMainState(const std::string &name = "unknown"):mName(name){};
	virtual ~IMainState(){};

	/**
	 * Set/Get name
	 */
	inline void setName(const std::string &name);
	inline const std::string &getName(void) const;

	/**
	 * Entering the state with additional info
	 */
	virtual void enter(const MainMachineInfo &info) = 0;

	/**
	 * Update the state... This function will be called once.
	 * @param	info	The structure used to pass throw the states. If we want
	 * 					to fill it with some information then we can do it.
	 * @return	event	The event that was happend.
	 */
	virtual MainMachineEvent update(MainMachineInfo &info) = 0;

	/**
	 * Function called when the state is not "the actual" anymore
	 */
	virtual void exit(void) = 0;

private:
	std::string		mName;
};


inline void IMainState::setName(const std::string &name)
{
	mName = name;
}
inline const std::string &IMainState::getName(void) const
{
	return mName;
}

#endif /* IMAINSTATE_H_ */
