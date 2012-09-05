/*
 * SMTransitionTable.h
 *
 *  Created on: 09/02/2012
 *      Author: agustin
 *
 */

#ifndef SMTRANSITIONTABLE_H_
#define SMTRANSITIONTABLE_H_


#include "MultiplatformTypedefs.h"
#include "DebugUtil.h"
#include "SMDefines.h"
#include "IState.h"
#include <iostream>


#define	StarState 0


template<typename T>
class SMTransitionTable {
	/* Multiplatform hash tables (LH was here) */
	typedef typename _HashTable<int, IState<T> *>::HashTable HashTable;
	typedef typename _HashTable<int, HashTable>::HashTable HashOfHashTable;

public:
	SMTransitionTable() :
		mStartState(0)
	{

	}

	// Destroy all the events
	~SMTransitionTable()
	{
		removeMemory();
	}

	/* Add new entry to the table
	 * Once a state is added here, we are the owners of the memory, so
	 * we will destroy the States when this table is destroyed
	 */
	void addNewEntry(IState<T> *from, SMEvent event, IState<T> *to)
	{
		// check if exists?
		ASSERT(getNextState(from, event) == 0);

		if(from == StarState){
			mStarTable.insert(std::pair<int, IState<T> *>(event,to));
		}else{
			mTable[from->getID()].insert(std::pair<int, IState<T> *>(event, to));
		}
	}

	/* Clear all the entries */
	void clearTable(void)
	{
		typename HashOfHashTable::iterator it = mTable.begin();
		for(; it != mTable.end(); ++it){
			typename HashTable::iterator it2 = it->second.begin();
			for(; it2 != it->second.end(); ++it2){
				delete it2->second;
			}
		}
		mTable.clear();

		typename HashTable::iterator it3 = mStarTable.begin();
		for(;it3 != mStarTable.end(); ++it3){
			delete it3->second;
		}
		mStarTable.clear();
	}

	/* Sets/Gets the start state */
	inline void setStartState(IState<T> *s) {ASSERT(s); mStartState = s;}
	inline IState<T> *getStartState(void) const {return mStartState;}

	/* Function used to get the next event given the actual state and some event
	 * In case that the event doesnt exists, 0 is returned
	 */
	inline IState<T> *getNextState(IState<T> *from, SMEvent event) const
	{

		typename HashTable::const_iterator it3 = mStarTable.find(event);
		if(it3 != mStarTable.end()){
			return it3->second;
		}else if(from){
		// Si no lo encontre en la tabla estrella y from no es estado estrella
			typename HashOfHashTable::const_iterator it = mTable.find(from->getID());
			// TODO: podemos optimizar esto metiendo asserts y a la mierda para
			// el debug, despues los sacamos y anda mas rapido

			if(it == mTable.end()){
				return 0;
			}
			// else return the istate of that
			typename HashTable::const_iterator it2 = it->second.find(event);
			if(it2 == it->second.end()){
				return 0;
			}
			return it2->second;
		}
		return 0;
	}

	/* returns if the table is empty */
	inline bool isEmpty(void) const {return mTable.empty();}

private:
	// remove memory
	void removeMemory(void)
	{
	}



private:
	IState<T>			*mStartState;
	HashOfHashTable		mTable;
	HashTable			mStarTable;
};

#endif /* SMTRANSITIONTABLE_H_ */
