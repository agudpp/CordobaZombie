/*
 * Loader.h
 *
 *  Created on: 27/09/2011
 *      Author: agustin
 */

#ifndef LOADER_H_
#define LOADER_H_

#include <string>

#include "LoaderData.h"
#include "CommonMath.h"
#include "tinyxml.h"
#include "DebugUtil.h"

class TiXmlElement;


class Loader
{
public:
	class LoaderCallback {
	public:
		virtual void operator()(float, const std::string&) = 0;
	};

public:
	Loader(const std::string &name = "") :
		mName(name),
		mTotalWeight(0),
		mChunkWeight(0.0f),
		mCallback(0)
	{

	}
	virtual ~Loader() {};

	// Set/Get the Loader's name.
	inline const std::string &getName(void) const;
	inline void setName(const std::string &n);

	// Set/Get the Loader's total weight.
	inline int getWeight(void) const;
	inline void setWeight(int w);

	// Set/Get the percentage of the total weight added by each loaded item.
	inline float getChunkWeight(void) const;
	virtual void setChunkWeight(TiXmlElement* elem) = 0;

	// Load our items.
	virtual int load(TiXmlElement* elem, LoaderData* data) = 0;

	// Unload our items.
	virtual int unload() = 0;

	// Set the callback to return information about the loading status.
	inline void setCallback(LoaderCallback *callback);

protected:
	/**
	 * Parse auxiliaries
	 */
	void parseAABB(TiXmlElement *elem, sm::AABB &aabb);
	void parseVector(TiXmlElement *elem, sm::Vector2 &vec);
	float			mChunkWeight;
	void**			mCallback;	// This is a LoaderManager::LoaderCallback,
								// but C++ can't forward declare nested classes.
private:
	std::string 	mName;
	int				mTotalWeight;
};


////////////////////////////////////////////////////////////////////////////////
inline const std::string& Loader::getName(void) const { return mName; }

////////////////////////////////////////////////////////////////////////////////
inline void Loader::setName(const std::string &n) { mName = n; }

////////////////////////////////////////////////////////////////////////////////
inline int Loader::getWeight(void) const { return mTotalWeight; }

////////////////////////////////////////////////////////////////////////////////
inline void Loader::setWeight(int w) { mTotalWeight = w; }

////////////////////////////////////////////////////////////////////////////////
inline float Loader::getChunkWeight(void) const { return mChunkWeight; }

////////////////////////////////////////////////////////////////////////////////
inline void Loader::setCallback(LoaderCallback *callback)
{
	ASSERT(callback);
	mCallback = (void**) callback;
}



#endif /* LOADER_H_ */
