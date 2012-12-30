/* This class will read an XML and will call for every child element the
 * corresponding loader (that will have to have the same name that the xml element)
 * See the sample.xml
 *
 *
 * LoaderManager.h
 *
 *  Created on: 27/09/2011
 *      Author: agustin
 */

#ifndef LOADERMANAGER_H_
#define LOADERMANAGER_H_

#include <OgreResourceManager.h>
#include <OgreString.h>
#include <string>
#include <vector>
#include <algorithm>

#include "DebugUtil.h"
#include "tinyxml.h"
#include "LoaderData.h"
#include "Loader.h"

class TiXmlDocument;

class LoaderManager
{
public:
	class LoaderCallback {
	public:
		virtual void operator()(float, const std::string&) = 0;
	};

private:
	struct Updater : public Loader::LoaderCallback {
		void operator()(float, const std::string&);
		void setCallback(LoaderCallback* lcb);
		float	mCurrentLoaderWeight;
	private:
		LoaderCallback*	mCallback;
	};

public:
	LoaderManager();

	// This class does NOT free any memory.
	~LoaderManager();

	// add new loader
	void addLoader(Loader *l);

	void removeLoader(Loader *l);
	void removeLoader(const std::string &name);
	void removeAll(void);
	void deleteAll(void);

	// Set the Callback to receive information about the loader that was already
	// "finished" of loading
	inline void setCallback(LoaderCallback *callback);

	// Get the sum of weights of all the Loaders that this Manager has
	int getSumOfWeights(void) const;

	// Get the loaders
	std::vector<Loader *> &getLoaders(void) {return mLoaders;}

	// Add element to load
	void addElement(const TiXmlElement *);
	void addElement(const Ogre::String &resource);

	// Removes elements
	void removeElement(const TiXmlElement *);
	void removeElement(const Ogre::String &resource);
	void removeAllElements(void);

	// TODO: falta hacer el cargado de datos desde un string.
	// solo vamos a implementar
	// Load all the data added before (all the elements added)
	int load(void);

	// Unload all the data of every Loader (calls the unload funcion)
	int unload(void);

	// Returns the loader data to configure it
	inline LoaderData &getLoaderData(void);

private:
	// Returns true if exists the loader and returns by params
	bool getLoader(const std::string &name, Loader *&loader);

	// Load the TiXmlElement
	TiXmlDocument *getTiXmlElement(const char *fname);

private:
	std::vector<Loader *>				mLoaders;
	std::vector<const TiXmlElement *>	mXmlElements;
	std::vector<Ogre::String>			mStrElements;
	LoaderCallback						*mCallback;
	LoaderData							mLoaderData;
	Updater								mUpdater;

};


////////////////////////////////////////////////////////////////////////////////
inline void LoaderManager::Updater::setCallback(LoaderCallback* lcb)
{
	ASSERT(lcb);
	mCallback = lcb;
}

////////////////////////////////////////////////////////////////////////////////
inline void LoaderManager::setCallback(LoaderCallback *callback)
{
	ASSERT(callback);
	mCallback = callback;
}

////////////////////////////////////////////////////////////////////////////////
inline LoaderData &LoaderManager::getLoaderData(void)
{
	return mLoaderData;
}

#endif /* LOADERMANAGER_H_ */
