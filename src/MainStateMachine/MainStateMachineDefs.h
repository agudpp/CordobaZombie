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
