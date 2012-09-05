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

class TiXmlElement;

class Loader {
public:
	Loader(const std::string &name = "") :
		mName(name),
		mWeight(0)
	{

	}
	virtual ~Loader() {};

	// Set/Get the weight
	inline int getWeight(void) const;
	inline void setWeight(int w);

	void setName(const std::string &n) {mName = n;}
	const std::string &getName(void) const {return mName;}

	// Functoin used to load something
	virtual int load(TiXmlElement*, LoaderData *data) = 0;

	// Unload the information?
	virtual int unload() = 0;

protected:
	/**
	 * Parse auxiliaries
	 */
	void parseAABB(TiXmlElement *elem, sm::AABB &aabb);
	void parseVector(TiXmlElement *elem, sm::Vector2 &vec);


private:
	std::string 		mName;
	int					mWeight;

};


inline int Loader::getWeight(void) const
{
	return mWeight;
}
inline void Loader::setWeight(int w)
{
	mWeight = w;
}


#endif /* LOADER_H_ */
