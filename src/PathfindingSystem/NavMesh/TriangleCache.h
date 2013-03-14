/*
 * TriangleCache.h
 *
 *  Created on: 18/06/2012
 *      Author: raul
 */



#ifndef TRIANGLECACHE_H
#define TRIANGLECACHE_H

#include "MultiplatformTypedefs.h"
#include "Graph.h"
#include <list>
#include "DebugUtil.h"



class TriangleCache{
private:
	// Functor to use as hash function below in 'GNodeToPath'.
	class sHash{
		public:
	    size_t operator ()(const GNode *gn) const{
	    	return reinterpret_cast<size_t>(gn);
	    }
	};
	typedef std::vector < const GNode* > Path;
	typedef _HashTable<const GNode *, Path*, sHash>::HashTable GNodeToPath;
	typedef _HashTable<const GNode *, GNodeToPath>::HashTable GNodeToGNodeToPath;
	typedef std::list <Path *> PathTable;
	PathTable										mTable;
	GNodeToGNodeToPath								mCache;
	int												mMaxSize;
	int												mMaxCount;

public:
	TriangleCache(int maxPathSize = 50, int maxCacheSize = 50);
	~TriangleCache();

	/*
	 * @addPath:
	 * Adds the Path 'path' to the cashe. 'from is the first node in the path
	 * and 'to' is the last one.
	 */
	void addPath( const GNode *from, const GNode *to,
			const GNode **path);

	/*
	 * @getPath:
	 * Returns the path from 'from' to 'to' if found in the cache.
	 * 'frwd' indicates if the path should be read from the front
	 * to the back or backwards.
	 */
	const GNode** getPath( const GNode *from, const GNode *to, bool &frwd,
			size_t &len);

	/*
	 * Like getPath but silent (doesn't refresh the path if founded)
	 */
	const GNode** findPath( const GNode *from, const GNode *to, bool &frwd,
			size_t &len) ;

	/*
	 * @clean:
	 * Limpia toda la cache (resetea). No toca la tabla sin embargo.
	 */
	inline void clear(void);

private:

	/*
	 * @addNewPath
	 * Adds 'path' to the Cache
	 */
	void addNewPath( const GNode *from, const GNode *to,
			const GNode **path);

	/*
	 * @refreshPath
	 * Moves 'path' to the end of the list, so it won't be deleted to soon
	 */
	inline void refreshPath( const GNode *from, const GNode *to,
			Path *path);

	/*
	 * @removePath
	 * Removes the oldest path in the Cache (the one that is at front of the
	 * table).
	 */
	void removePath(void);
};


/*******************************************************************************
*                                                                              *
*******************************************************************************/

inline void TriangleCache::refreshPath(const GNode *from, const GNode *to,
		Path *path)
{

	mTable.remove(path);
	mTable.push_back(path);
	mCache[from][to] = mTable.back();
	mCache[to][from] = mTable.back();

}


inline void TriangleCache::clear(void)
{
	//ASSERT(mCache);
	GNodeToGNodeToPath::iterator it = mCache.begin();
	for(; it != mCache.end(); it++){
		it->second.clear();
	}
}

#endif
