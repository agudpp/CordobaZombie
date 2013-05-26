/*
 * MainTransitionFunction.h
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#ifndef MAINTRANSITIONFUNCTION_H_
#define MAINTRANSITIONFUNCTION_H_

#include <map>

#include "DebugUtil.h"
#include "MainStateMachineDefs.h"
#include "IMainState.h"

class MainTransitionFunction
{
	typedef std::map<MainMachineEvent, IMainState *>	MapTable;
	typedef std::map<IMainState *, MapTable>		MapMapTable;
public:
	MainTransitionFunction();
	~MainTransitionFunction();


	/* Add new entry to the table
	 * Once a state is added here, we are the owners of the memory, so
	 * we will destroy the States when this table is destroyed
	 */
	void addNewEntry(IMainState *from, MainMachineEvent event, IMainState *to);

	/* Clear all the entries */
	void clearTable(void);

	/* Sets/Gets the start state */
	inline void setStartState(IMainState *s);
	inline IMainState *getStartState(void) const;

	/* Function used to get the next event given the actual state and some event
	 * In case that the event doesnt exists, 0 is returned
	 */
	IMainState *getNextState(IMainState *from, MainMachineEvent event) const;

	/* returns if the table is empty */
	inline bool isEmpty(void) const;

private:
	// remove memory
	void removeMemory(void);


private:
	IMainState			*mStartState;
	MapMapTable			mTable;
};



inline void MainTransitionFunction::setStartState(IMainState *s)
{
	ASSERT(s);
	mStartState = s;
}

inline IMainState *MainTransitionFunction::getStartState(void) const
{
	return mStartState;
}

inline bool MainTransitionFunction::isEmpty(void) const
{
	return mTable.empty();
}



#endif /* MAINTRANSITIONFUNCTION_H_ */
