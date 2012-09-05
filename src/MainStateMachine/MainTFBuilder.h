/*
 * MainTFBuilder.h
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#ifndef MAINTFBUILDER_H_
#define MAINTFBUILDER_H_


#include "MainTransitionFunction.h"
#include "MainStateMachineDefs.h"
#include "tinyxml.h"


class TiXmlElement;
class LoaderManager;

class MainTFBuilder
{
public:
	MainTFBuilder();
	~MainTFBuilder();

	/**
	 * Set the loader manager
	 */
	inline void setLoaderManager(LoaderManager *lm);

	/**
	 * Build Transition function from xml?
	 */
	MainTransitionFunction *build(const TiXmlElement *elem);

private:
	LoaderManager		*mLoaderManager;
};


inline void MainTFBuilder::setLoaderManager(LoaderManager *lm)
{mLoaderManager = lm;}


#endif /* MAINTFBUILDER_H_ */
