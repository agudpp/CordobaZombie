/*
 * TriangleCache.cpp
 *
 *  Created on: 18/06/2012
 *      Author: raul
 */

#include "TriangleCache.h"


TriangleCache::TriangleCache(int maxPathSize, int maxCacheSize):
mMaxCount(maxCacheSize),
mMaxSize(maxPathSize)
{
	ASSERT(maxPathSize >= 0 && maxCacheSize >= 0);

	mTable.resize(maxCacheSize);
	PathTable::iterator it = mTable.begin();

	for(; it != mTable.end(); it++){
		// +1 for the Null GNode indicating the end of the path
		(*it) = new Path;
		(*it)->reserve(maxPathSize+1);
		(*it)->clear();
	}

#ifdef DEBUG_CRITICAL
	debugRAUL("Cache size %d of paths size %d created\n",
			mTable.size(), maxPathSize);
#endif
}

TriangleCache::~TriangleCache()
{
	PathTable::iterator it = mTable.begin();
		for(; it != mTable.end(); it++){
			// +1 for the Null GNode indicating the end of the path
			delete (*it);
		}
}


void TriangleCache::removePath(void)
{
	Path *aux = mTable.front();
	Path & front = (*aux);

	ASSERT(aux);
	ASSERT(front.size() >= 0);

	if(!front.size()) return;

	const GNode *f = front[0];
	int i = 0;
	const GNode *t = front[front.size()-1];

	GNodeToGNodeToPath::iterator it = mCache.find(f);

	if (it != mCache.end()){
		GNodeToPath & hash = it->second;
		hash.erase(t);
	}

	it = mCache.find(t);
	if(it != mCache.end()){
		GNodeToPath & hash = it->second;
		hash.erase(f);
	}
}



void TriangleCache::addNewPath( const GNode *from, const GNode *to,
		const GNode **path)
{
	ASSERT(from);
	ASSERT(to);
	ASSERT(path);
	// remove first from cache
	removePath();
	// dont lose the reference (aux)
	Path *aux = (mTable.front());
	aux->resize(mMaxSize);
	// remove first from table
	mTable.pop_front();
	int j = 0;
	while(path[j] != 0){
		(*aux)[j] = path[j];
		j++;
	}
	aux->resize(j);

	mTable.push_back(aux);

	mCache[from][to] = aux;
	mCache[to][from] = aux;

}


const GNode**
TriangleCache::getPath(const GNode *from,
                       const GNode *to,
                       bool &frwd,
                       size_t &len)
{
	ASSERT(from);
	ASSERT(to);

	frwd = false;

	GNodeToGNodeToPath::iterator f1 = (mCache.find(from));
	if (f1 != mCache.end()){
		GNodeToPath & gtp = f1->second;
		GNodeToPath::iterator f2 = gtp.find(to);
		if(f2 != gtp.end()){
			// I have it
			const GNode **ret = &((*(f2->second))[0]);
			if (from == ret[0]) frwd = true;
			refreshPath(from,to,f2->second); // refresh the path
			len = (f2->second)->size();
			return ret;
		}
	}

	f1 = (mCache.find(to));
	if (f1 != mCache.end()){
		GNodeToPath & gtp = f1->second;
		GNodeToPath::iterator f2 = gtp.find(from);
		if(f2 != gtp.end()){
			// I have it
			const GNode **ret = &((*(f2->second))[0]);
			if (from == ret[0]) frwd = true;
			refreshPath(from,to,f2->second);
			len = (f2->second)->size();
			return ret;
		}
	}

	//dont have it
	return 0;
}


const GNode**
TriangleCache::findPath(const GNode *from,
                        const GNode *to,
                        bool &frwd,
                        size_t &len)
{
	ASSERT(from);
	ASSERT(to);

	frwd = false;

	GNodeToGNodeToPath::iterator f1 = (mCache.find(from));
	if (f1 != mCache.end()){
		GNodeToPath & gtp = f1->second;
		GNodeToPath::iterator f2 = gtp.find(to);
		if(f2 != gtp.end()){
			// I have it
			const GNode **ret = &((*(f2->second))[0]);
			if (from == ret[0]) frwd = true;
			len = (f2->second)->size();
			return ret;
		}
	}

	f1 = (mCache.find(to));
	if (f1 != mCache.end()){
		GNodeToPath & gtp = f1->second;
		GNodeToPath::iterator f2 = gtp.find(from);
		if(f2 != gtp.end()){
			// I have it
			const GNode **ret = &((*(f2->second))[0]);
			if (from == ret[0]) frwd = true;
			len = (f2->second)->size();
			return ret;
		}
	}

	//dont have it
	return 0;
}


void TriangleCache::addPath(const GNode *from, const GNode *to,
		const GNode **path)
{
	ASSERT(from);
	ASSERT(to);
	ASSERT(path);
	GNodeToGNodeToPath::iterator f1 = (mCache.find(from));
	if (f1 != mCache.end()){
		GNodeToPath & gtp = (*f1).second;
		GNodeToPath::iterator f2 = gtp.find(to);
		if(f2 != gtp.end()){
			// I already have it, refresh it:
			refreshPath(from, to, f2->second);
			return;
		}
	}

	f1 = (mCache.find(to));
	if (f1 != mCache.end()){
		GNodeToPath & gtp = (*f1).second;
		GNodeToPath::iterator f2 = gtp.find(from);
		if(f2 != gtp.end()){
			// I already have it, refresh it:
			refreshPath(to, from, f2->second);
			return;
		}
	}

	// We dont have it, we add it:
	addNewPath(from, to, path);
	return;
}

