/*
 * MainStateMachineDefs.h
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#ifndef MAINSTATEMACHINEDEFS_H_
#define MAINSTATEMACHINEDEFS_H_

#include <string>
#include <map>

/**
 * Here we will define some of the params we will use in the different states
 * inside the MainMachineParams structure:
 * LEVEL_PATH = will be the current level Path where all the resources for that
 *              level should be (loading info, geometry info, story info, etc).
 *              Inside this path we should separate into folders for this different
 *              assets. We should reproduce the folder hierarchy for different
 *              levels.
 */


typedef std::map<std::string, std::string>		MainMachineParams;


typedef struct {
	int					flags;
	void				*data;
	MainMachineParams	params;
} MainMachineInfo;


// DEFINE the events
enum MainMachineEvent {
	MME_ERROR = -1,
	MME_DONE = 0,
	MME_EXIT,


};


#endif /* MAINSTATEMACHINEDEFS_H_ */
