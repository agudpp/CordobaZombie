/*
 * QueueContainer.h
 *
 *  Created on: 12/04/2012
 *      Author: agustin
 *
 */

#ifndef QUEUECONTAINER_H_
#define QUEUECONTAINER_H_


#include <deque>
#include "DebugUtil.h"

template<typename T>
class QueueContainer {
public:
	QueueContainer(){};
	~QueueContainer()
	{
//		debugRED("TODO: deberiamos borrar todos los objetos...\n");
		freeAll();
	}

	/**
	 * Returns the queue
	 */
	inline std::deque<T*> &getQueue(void){return mQueue;}

	/**
	 * Get a new available object
	 */
	inline T* getAvailableObject(void)
	{
		if(mQueue.empty()){return 0;}
		T* obj = mQueue.front();
		mQueue.pop_front();
		return obj;
	}

	/**
	 * Add a new object to the container
	 */
	inline void addNewObject(T *obj)
	{
		mQueue.push_back(obj);
	}

	/**
	 * Remove and free the memory of all elements
	 */
	inline void freeAll(void)
	{
		typename std::deque<T*>::iterator bit = mQueue.begin(),
				eit = mQueue.end();
		for(; bit != eit; ++bit){
			delete *bit;
		}
		mQueue.clear();
	}

private:
	std::deque<T*>		mQueue;
};

#endif /* QUEUECONTAINER_H_ */
