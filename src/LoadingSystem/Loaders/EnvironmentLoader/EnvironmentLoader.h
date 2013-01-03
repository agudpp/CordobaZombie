/*
 * EnvironmentLoader.h
 *
 *  Created on: 29/12/2012
 *      Author: agustin
 *
 */

#ifndef ENVIRONMENTLOADER_H_
#define ENVIRONMENTLOADER_H_

#include <vector>

#include <OgreLight.h>

#include "Loader.h"

class TiXmlElement;

class EnvironmentLoader : public Loader {
    static const int ENVIRONMENT_WEIGHT = 4;
    typedef std::vector<Ogre::Light *> LightsVec;
public:
    EnvironmentLoader();
    virtual ~EnvironmentLoader();

    // Functoin used to load something
    virtual int load(TiXmlElement*, LoaderData *data);

    // Unload the information?
    virtual int unload();


private:
    // Happy to duplicate the code!!! FUCK!
    //
    void processFog(TiXmlElement *XMLNode);
    void processSkyBox(TiXmlElement *XMLNode);
    void processSkyDome(TiXmlElement *XMLNode);
    void processSkyPlane(TiXmlElement *XMLNode);
    void processClipping(TiXmlElement *XMLNode);
    void processEnvironment(TiXmlElement *XMLNode);

    void processLightRange(TiXmlElement *XMLNode, Ogre::Light *pLight);
    void processLightAttenuation(TiXmlElement *XMLNode, Ogre::Light *pLight);
    void processLights(TiXmlElement *XMLNode);


private:
    LightsVec   mLights;
};

#endif /* ENVIRONMENTLOADER_H_ */
