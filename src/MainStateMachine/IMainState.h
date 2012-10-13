/*
 * IMainState.h
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#ifndef IMAINSTATE_H_
#define IMAINSTATE_H_

#include <string>
#include <vector>

#include "MainStateMachineDefs.h"

#ifdef DEBUG
#include "DebugUtil.h"
#endif

class IMainState
{
public:

    struct ResourcesInfo {
        std::string filePath;
        std::vector<std::string> groupNames;
    };

    typedef std::vector<IMainState::ResourcesInfo> ResourcesInfoVec;

public:
	IMainState(const std::string &name = "unknown"):mName(name){};
	virtual ~IMainState(){};

	/**
	 * Set/Get name
	 */
	inline void setName(const std::string &name);
	inline const std::string &getName(void) const;

	/**
	 * Function used to get the resources files used by the state.
	 * The list returned is the list of the resources used by and only by this
	 * state.
	 */
	virtual void getResources(ResourcesInfoVec &resourcesList)
	{
	    debugERROR("Esta clase tiene que estar reimplementada en las clases que "
	            "heredan, en este caso el estado %s no lo tiene implementado\n",
	            mName.c_str());
	    resourcesList.clear();
	}

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
