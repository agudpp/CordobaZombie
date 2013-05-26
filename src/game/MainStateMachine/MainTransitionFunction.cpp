/*
 * MainTransitionFunction.cpp
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#include "MainTransitionFunction.h"

MainTransitionFunction::MainTransitionFunction()
{

}

MainTransitionFunction::~MainTransitionFunction()
{
	removeMemory();
}


/* Add new entry to the table
 * Once a state is added here, we are the owners of the memory, so
 * we will destroy the States when this table is destroyed
 */
void MainTransitionFunction::addNewEntry(IMainState *from,
		MainMachineEvent event, IMainState *to)
{
	// check if exists?
	ASSERT(getNextState(from, event) == 0);
	mTable[from].insert(std::pair<MainMachineEvent, IMainState *>(event, to));
}

/* Clear all the entries */
void MainTransitionFunction::clearTable(void)
{
	removeMemory();
	mTable.clear();
}


/* Function used to get the next event given the actual state and some event
 * In case that the event doesnt exists, 0 is returned
 */
IMainState *MainTransitionFunction::getNextState(IMainState *from,
		MainMachineEvent event) const
{
	ASSERT(from);
	MapMapTable::const_iterator it = mTable.find(from);

	// TODO: podemos optimizar esto metiendo asserts y a la mierda para
	// el debug, despues los sacamos y anda mas rapido

	if(it == mTable.end()){
		return 0;
	}
	// else return the istate of that
	MapTable::const_iterator it2 = it->second.find(event);
	if(it2 == it->second.end()){
		return 0;
	}

	return it2->second;
}

// remove memory
void MainTransitionFunction::removeMemory(void)
{
	MapMapTable::iterator it = mTable.begin();
	for(; it != mTable.end(); ++it){
		MapTable::iterator it2 = it->second.begin();
		for(; it2 != it->second.end(); ++it2){
			delete it2->second;
		}
	}
	mTable.clear();
}


