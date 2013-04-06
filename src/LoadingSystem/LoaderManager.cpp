/*
 * LoaderManager.cpp
 *
 *  Created on: 27/09/2011
 *      Author: agustin
 */
#include "LoaderManager.h"
#include "tinyxml.h"
#include "Util.h"
#include "Loader.h"



////////////////////////////////////////////////////////////////////////////////
void LoaderManager::Updater::operator()(float w, const std::string& msg)
{
	/* The weight parameter 'w' is a value in [0,1],
	 * representing the chunk of data the current Loader has just loaded. */
	ASSERT(mCallback);
	(*mCallback)(mCurrentLoaderWeight*w, msg);
}

////////////////////////////////////////////////////////////////////////////////
TiXmlDocument *LoaderManager::getTiXmlElement(const char *fname)
{
	return Common::Util::loadXmlDocument(fname);
}

////////////////////////////////////////////////////////////////////////////////
bool LoaderManager::getLoader(const std::string &name, Loader *&loader)
{
	for(int i = mLoaders.size()-1; i >= 0; --i){
		if(mLoaders[i]->getName() == name){
			loader = mLoaders[i];
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////
LoaderManager::LoaderManager() { /* Auto-generated constructor stub */ }

////////////////////////////////////////////////////////////////////////////////
LoaderManager::~LoaderManager() { /* Auto-generated destructor stub */ }


////////////////////////////////////////////////////////////////////////////////
void LoaderManager::addLoader(Loader *l)
{
	ASSERT(l);
	std::vector<Loader *>::iterator it = std::find(mLoaders.begin(), mLoaders.end(), l);
	ASSERT(it == mLoaders.end());
	l->setCallback(&mUpdater);
	mLoaders.push_back(l);
}

////////////////////////////////////////////////////////////////////////////////
void LoaderManager::removeLoader(Loader *l)
{
	ASSERT(l);
	std::vector<Loader *>::iterator i = std::find(mLoaders.begin(),mLoaders.end(), l);
	ASSERT(i != mLoaders.end());

	mLoaders.erase(i);
}

////////////////////////////////////////////////////////////////////////////////
void LoaderManager::removeLoader(const std::string &name)
{
	int i = 0;
	for(i = mLoaders.size()-1; i >= 0; --i)
		if(mLoaders[i]->getName() == name)
			break;

	ASSERT(i>=0);

	mLoaders.erase(mLoaders.begin() + i);
}

////////////////////////////////////////////////////////////////////////////////
void LoaderManager::removeAll(void) { mLoaders.clear(); }

////////////////////////////////////////////////////////////////////////////////
void
LoaderManager::deleteAll(void)
{
    for(size_t size = mLoaders.size(), i = 0; i < size; ++i){
        mLoaders[i]->unload();
        delete mLoaders[i];
    }
    mLoaders.clear();
}

////////////////////////////////////////////////////////////////////////////////
int LoaderManager::getSumOfWeights(void) const
{
	int result = 0;
	for(std::vector<Loader *>::const_iterator it = mLoaders.begin();
			it != mLoaders.end(); ++it){
		result += (*it)->getWeight();
	}
	return result;
}


////////////////////////////////////////////////////////////////////////////////
void LoaderManager::addElement(const TiXmlElement *e)
{
	ASSERT(e);
	ASSERT(std::find(mXmlElements.begin(), mXmlElements.end(),e) == mXmlElements.end());
	mXmlElements.push_back(e);
}

////////////////////////////////////////////////////////////////////////////////
void LoaderManager::addElement(const Ogre::String &e)
{
	ASSERT(std::find(mStrElements.begin(), mStrElements.end(),e) == mStrElements.end());
	mStrElements.push_back(e);
}

////////////////////////////////////////////////////////////////////////////////
void LoaderManager::removeElement(const TiXmlElement *e)
{
	ASSERT(e);
	std::vector<const TiXmlElement *>::iterator it = std::find(mXmlElements.begin(),
														mXmlElements.end(),e);
	ASSERT(it != mXmlElements.end());

	mXmlElements.erase(it);
}

////////////////////////////////////////////////////////////////////////////////
void
LoaderManager::removeAllElements()
{
    mXmlElements.clear();
    mStrElements.clear();
}

////////////////////////////////////////////////////////////////////////////////
void LoaderManager::removeElement(const Ogre::String &e)
{
	std::vector<Ogre::String>::iterator it = std::find(mStrElements.begin(),
													   mStrElements.end(),e);
	ASSERT(it != mStrElements.end());
	mStrElements.erase(it);
}

////////////////////////////////////////////////////////////////////////////////
int LoaderManager::load(void)
{
	Loader *l = 0;
	const char *name = 0;

	// Load the XML elements
	for(int i = mXmlElements.size()-1; i >= 0; --i){
		const TiXmlElement *elem = mXmlElements[i];
		if(Ogre::String(elem->Value()) != "LoadingSystem"){
			debug("Invalid XML %s\n", elem->Value());
			continue;
		}

		elem = elem->FirstChildElement("Element");
		while(elem) {
			name = elem->Attribute("builderName");
			if(!name){
				debug("No builder set for %s\n", elem->Value());
				elem = elem->NextSiblingElement();
				ASSERT(false);
				continue;
			}

			if(!getLoader(name, l)){
				debug("No Loader with name %s was added\n", name);
				elem = elem->NextSiblingElement();
				ASSERT(false);
				continue;
			}

			// get the filename to be parsed
			const char *filename = elem->Attribute("fileName");
			if(!filename){
				debug("No filename set in %s\n", name);
				ASSERT(false);
				elem = elem->NextSiblingElement();
				continue;
			}

			// Set current Loader's weight in the Updater callback
			mUpdater.mCurrentLoaderWeight = l->getWeight();

			// we have to read the TiXmlElement
			TiXmlDocument *doc = getTiXmlElement(filename);
			if(!doc){
				elem = elem->NextSiblingElement();
				continue;
			}
			TiXmlElement *newXml = doc->RootElement();
			if(l->load(newXml, &mLoaderData) != 0){
				debug("Loader %s returns some error\n", name);
				delete doc;
				ASSERT(false);
				elem = elem->NextSiblingElement();
			}

			// removes the doc
			delete doc;

			elem = elem->NextSiblingElement();
		}
	}

	// TODO: falta hacer ahora el tema de los strings (deberiamos leer los
	// strings y luego generar el XMLDoc (TiXMLElement y empezar a parsear eso).

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int LoaderManager::unload(void)
{
	for(int i = mLoaders.size() - 1; i >= 0; --i){
		if(mLoaders[i]->unload() != 0){
			debug("Unloading error in %s\n", mLoaders[i]->getName().c_str());
			ASSERT(false);
			return -1;
		}
	}

	return 0;
}
