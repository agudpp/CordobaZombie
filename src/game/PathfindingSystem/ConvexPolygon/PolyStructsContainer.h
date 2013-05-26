/*
 * PolyStructsContainer.h
 *
 *  Created on: 20/01/2012
 *      Author: agustin
 *
 */

#ifndef POLYSTRUCTSCONTAINER_H_
#define POLYSTRUCTSCONTAINER_H_

#include <vector>

#ifdef DEBUG
#include <algorithm>
#include "DebugUtil.h"
#endif

#include "PolygonStructs.h"

template<class T>
class PolyStructsContainer {
	typedef std::vector<T>	ObjsVec;
public:
	PolyStructsContainer(){};
	~PolyStructsContainer(){removeAndFreeAll();};

	// Add new obj to track it
	void addObj(T v)
	{
	#ifdef DEBUG
		typename ObjsVec::iterator it = std::find(mObjs.begin(), mObjs.end(),v);
		if(it != mObjs.end()){
			ASSERT(false);
		}

	#endif
		mObjs.push_back(v);
	}

	// returns all the obj
	std::vector<T> &getObjs(void) {return mObjs;}

	const std::vector<T> &getObjs(void) const {return mObjs;}


	// Clear the objs vector (without freeing it)
	void clearAll(void)
	{
		mObjs.clear();
	}

	// Remove and free all the objs
	void removeAndFreeAll(void)
	{
		for(int i = mObjs.size() - 1; i >= 0; --i)
			delete mObjs[i];

		mObjs.clear();
	}

	// Check if is empty
	inline bool isEmpty(void) const {return mObjs.empty();}

	// Returns size
	inline size_t getSize(void) const {return mObjs.size();}


private:

	PolyStructsContainer(const PolyStructsContainer&);

private:
	ObjsVec				mObjs;


};

#endif /* POLYSTRUCTSCONTAINER_H_ */
