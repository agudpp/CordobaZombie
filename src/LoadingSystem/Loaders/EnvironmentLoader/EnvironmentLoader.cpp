/*
 * EnvironmentLoader.cpp
 *
 *  Created on: 29/12/2012
 *      Author: agustin
 *
 */

#include "EnvironmentLoader.h"

#include <OgreVector3.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreMovableObject.h>

#include "DebugUtil.h"
#include "GlobalObjects.h"
#include "DotSceneLoader.h"



////////////////////////////////////////////////////////////////////////////////
void
EnvironmentLoader::processFog(TiXmlElement *XMLNode)
{
    // Process attributes
    Ogre::Real expDensity = Ogre::DotSceneLoader::getAttribReal(
            XMLNode, "expDensity", 0.001);
    Ogre::Real linearStart = Ogre::DotSceneLoader::getAttribReal(
            XMLNode, "linearStart", 0.0);
    Ogre::Real linearEnd = Ogre::DotSceneLoader::getAttribReal
            (XMLNode, "linearEnd", 1.0);

    Ogre::FogMode mode = Ogre::FogMode::FOG_NONE;
    Ogre::String sMode = Ogre::DotSceneLoader::getAttrib(XMLNode, "mode");
    if(sMode == "none")
        mode = Ogre::FogMode::FOG_NONE;
    else if(sMode == "exp")
        mode = Ogre::FogMode::FOG_EXP;
    else if(sMode == "exp2")
        mode = Ogre::FogMode::FOG_EXP2;
    else if(sMode == "linear")
        mode = Ogre::FogMode::FOG_LINEAR;

    TiXmlElement *pElement;

    // Process colourDiffuse (?)
    Ogre::ColourValue colourDiffuse = Ogre::ColourValue::White;
    pElement = XMLNode->FirstChildElement("colourDiffuse");
    if(pElement)
        colourDiffuse = Ogre::DotSceneLoader::parseColour(pElement);

    // Setup the fog
    GLOBAL_SCN_MNGR->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}

void
EnvironmentLoader::processSkyBox(TiXmlElement *XMLNode)
{
    // Process attributes
    Ogre::String material = Ogre::DotSceneLoader::getAttrib(XMLNode, "material");
    Ogre::Real distance = Ogre::DotSceneLoader::getAttribReal(XMLNode, "distance", 5000);
    bool drawFirst = Ogre::DotSceneLoader::getAttribBool(XMLNode, "drawFirst", true);

    TiXmlElement *pElement;

    // Process rotation (?)
    Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
    pElement = XMLNode->FirstChildElement("rotation");
    if(pElement)
        rotation = Ogre::DotSceneLoader::parseQuaternion(pElement);

    // Setup the sky box
    GLOBAL_SCN_MNGR->setSkyBox(true, material, distance, drawFirst, rotation);
}

void
EnvironmentLoader::processSkyDome(TiXmlElement *XMLNode)
{
    // Process attributes
    Ogre::String material = XMLNode->Attribute("material");
    Ogre::Real curvature = Ogre::DotSceneLoader::getAttribReal(XMLNode, "curvature", 10);
    Ogre::Real tiling = Ogre::DotSceneLoader::getAttribReal(XMLNode, "tiling", 8);
    Ogre::Real distance = Ogre::DotSceneLoader::getAttribReal(XMLNode, "distance", 4000);
    bool drawFirst = Ogre::DotSceneLoader::getAttribBool(XMLNode, "drawFirst", true);

    TiXmlElement *pElement;

    // Process rotation (?)
    Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
    pElement = XMLNode->FirstChildElement("rotation");
    if(pElement)
        rotation = Ogre::DotSceneLoader::parseQuaternion(pElement);

    // Setup the sky dome
    GLOBAL_SCN_MNGR->setSkyDome(true, material, curvature, tiling, distance,
            drawFirst, rotation, 16, 16, -1);
}

void
EnvironmentLoader::processSkyPlane(TiXmlElement *XMLNode)
{
    // Process attributes
    Ogre::String material = Ogre::DotSceneLoader::getAttrib(XMLNode, "material");
    Ogre::Real planeX = Ogre::DotSceneLoader::getAttribReal(XMLNode, "planeX", 0);
    Ogre::Real planeY = Ogre::DotSceneLoader::getAttribReal(XMLNode, "planeY", -1);
    Ogre::Real planeZ = Ogre::DotSceneLoader::getAttribReal(XMLNode, "planeX", 0);
    Ogre::Real planeD = Ogre::DotSceneLoader::getAttribReal(XMLNode, "planeD", 5000);
    Ogre::Real scale = Ogre::DotSceneLoader::getAttribReal(XMLNode, "scale", 1000);
    Ogre::Real bow = Ogre::DotSceneLoader::getAttribReal(XMLNode, "bow", 0);
    Ogre::Real tiling = Ogre::DotSceneLoader::getAttribReal(XMLNode, "tiling", 10);
    bool drawFirst = Ogre::DotSceneLoader::getAttribBool(XMLNode, "drawFirst", true);

    // Setup the sky plane
    Ogre::Plane plane;
    plane.normal = Ogre::Vector3(planeX, planeY, planeZ);
    plane.d = planeD;
    GLOBAL_SCN_MNGR->setSkyPlane(true, plane, material, scale, tiling, drawFirst,
            bow, 1, 1);
}

void
EnvironmentLoader::processClipping(TiXmlElement *XMLNode)
{
    // Process attributes
    Ogre::Real fNear = Ogre::DotSceneLoader::getAttribReal(XMLNode, "near", 0);
    Ogre::Real fFar = Ogre::DotSceneLoader::getAttribReal(XMLNode, "far", 1);
    GLOBAL_CAMERA->setFarClipDistance(fFar);
    GLOBAL_CAMERA->setNearClipDistance(fNear);
}

void
EnvironmentLoader::processEnvironment(TiXmlElement *XMLNode)
{
    TiXmlElement *pElement;

    // Process fog (?)
    pElement = XMLNode->FirstChildElement("fog");
    if(pElement != 0){
        processFog(pElement);
    }

    // Process skyBox (?)
    pElement = XMLNode->FirstChildElement("skyBox");
    if(pElement != 0){
        processSkyBox(pElement);
    }

    // Process skyDome (?)
    pElement = XMLNode->FirstChildElement("skyDome");
    if(pElement != 0){
        processSkyDome(pElement);
    }

    // Process skyPlane (?)
    pElement = XMLNode->FirstChildElement("skyPlane");
    if(pElement != 0){
        processSkyPlane(pElement);
    }

    // Process clipping (?)
    pElement = XMLNode->FirstChildElement("clipping");
    if(pElement != 0){
        processClipping(pElement);
    }

    // Process colourAmbient (?)
    pElement = XMLNode->FirstChildElement("colourAmbient");
    if(pElement != 0){
        GLOBAL_SCN_MNGR->setAmbientLight(Ogre::DotSceneLoader::parseColour(pElement));
    }

}

void
EnvironmentLoader::processLightRange(TiXmlElement *XMLNode, Ogre::Light *pLight)
{
    // Process attributes
    Ogre::Real inner = Ogre::DotSceneLoader::getAttribReal(XMLNode, "inner");
    Ogre::Real outer = Ogre::DotSceneLoader::getAttribReal(XMLNode, "outer");
    Ogre::Real falloff = Ogre::DotSceneLoader::getAttribReal(XMLNode, "falloff", 1.0);

    // Setup the light range
    pLight->setSpotlightRange(Ogre::Angle(inner), Ogre::Angle(outer), falloff);
}

void
EnvironmentLoader::processLightAttenuation(TiXmlElement *XMLNode, Ogre::Light *pLight)
{
    // Process attributes
    Ogre::Real range = Ogre::DotSceneLoader::getAttribReal(XMLNode, "range");
    Ogre::Real constant = Ogre::DotSceneLoader::getAttribReal(XMLNode, "constant");
    Ogre::Real linear = Ogre::DotSceneLoader::getAttribReal(XMLNode, "linear");
    Ogre::Real quadratic = Ogre::DotSceneLoader::getAttribReal(XMLNode, "quadratic");

    // Setup the light attenuation
    pLight->setAttenuation(range, constant, linear, quadratic);
}

void
EnvironmentLoader::processLight(TiXmlElement *XMLNode)
{
    // Process attributes
    TiXmlElement *lightNode = XMLNode->FirstChildElement("light");
    if (lightNode == 0) {
        debugERROR("No light node for %s\n", XMLNode->Value());
        return;
    }

    Ogre::String name = Ogre::DotSceneLoader::getAttrib(lightNode, "name");

    // Create the light
    Ogre::Light *pLight = GLOBAL_SCN_MNGR->createLight(name);
    mLights.push_back(pLight);

    Ogre::String sValue = Ogre::DotSceneLoader::getAttrib(lightNode, "type");
    if(sValue == "point")
        pLight->setType(Ogre::Light::LT_POINT);
    else if(sValue == "directional")
        pLight->setType(Ogre::Light::LT_DIRECTIONAL);
    else if(sValue == "spot")
        pLight->setType(Ogre::Light::LT_SPOTLIGHT);
    else if(sValue == "radPoint")
        pLight->setType(Ogre::Light::LT_POINT);

    pLight->setVisible(Ogre::DotSceneLoader::getAttribBool(lightNode, "visible", true));
    pLight->setCastShadows(Ogre::DotSceneLoader::getAttribBool(lightNode, "castShadows", true));
    pLight->setPowerScale(Ogre::DotSceneLoader::getAttribReal(lightNode, "power", 1.0f));

    TiXmlElement *pElement;

    debugWARNING("We are using the position of the node... it seems that the "
            "position of the light is not used anymore?\n");
    // Process position (?)
    pElement = XMLNode->FirstChildElement("position");
    if(pElement) {
        pLight->setPosition(Ogre::DotSceneLoader::parseVector3(pElement));
    }

    debugWARNING("There are no normal!!! to parse... the direction of the light"
            " must be calculated manually? using the target node?\n");
    // Process normal (?)
    pElement = XMLNode->FirstChildElement("normal");
    if(pElement) {
        pLight->setDirection(Ogre::DotSceneLoader::parseVector3(pElement));
    }

    // Process colourDiffuse (?)
    pElement = lightNode->FirstChildElement("colourDiffuse");
    if(pElement) {
        pLight->setDiffuseColour(Ogre::DotSceneLoader::parseColour(pElement));
    }

    // Process colourSpecular (?)
    pElement = lightNode->FirstChildElement("colourSpecular");
    if(pElement) {
        pLight->setSpecularColour(Ogre::DotSceneLoader::parseColour(pElement));
    }

    // Process lightRange (?)
    pElement = lightNode->FirstChildElement("lightRange");
    if(pElement) {
        processLightRange(pElement, pLight);
    }

    // Process lightAttenuation (?)
    pElement = lightNode->FirstChildElement("lightAttenuation");
    if(pElement) {
        processLightAttenuation(pElement, pLight);
    }
}

void
EnvironmentLoader::processLights(TiXmlElement *XMLNode)
{
    // process lights
    TiXmlElement *pElement = XMLNode->FirstChildElement("node");
    if (pElement == 0) {
        debugWARNING("There are no lights? or the xml is invalid? %s\n",
                XMLNode->Value());
        return;
    }

    // iterate over all the nodes
    while (pElement != 0) {
        ASSERT(false); // tenemos que ver como se guardan las lights en un .scene
        processLight(pElement);
        pElement = pElement->NextSiblingElement("node");
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EnvironmentLoader::EnvironmentLoader() :
    Loader("EnvironmentLoader")
{
    setWeight(ENVIRONMENT_WEIGHT);
}

EnvironmentLoader::~EnvironmentLoader()
{
}

// Functoin used to load something
int
EnvironmentLoader::load(TiXmlElement* elem, LoaderData *data)
{
    ASSERT(elem != 0);
    ASSERT(data != 0);
    ASSERT(mLights.empty());

    // verify if the we have a correct xml
    debugGREEN("Environment loader is loading\n");
    if( Ogre::String( elem->Value()) != "EnvironmentInfo"  ) {
        debugERROR("Error: Invalid EnvironmentInfo File. Missing <EnvironmentInfo>" );
        ASSERT(false);
        return -1;
    }

    // process environment first
    TiXmlElement *pElement = 0;
    pElement = elem->FirstChildElement("environment");
    if(pElement != 0){
        processEnvironment(pElement);
    }
    // TODO: integrate with carlox changes here

    // process the lights if we have..
    pElement = elem->FirstChildElement("lights");
    if (pElement != 0) {
        processLights(pElement);
    }

    debugGREEN("All environment stuff were loaded\n");
    return 0;
}

// Unload the information?
int
EnvironmentLoader::unload()
{
    for (size_t i = 0, size = mLights.size(); i < size; ++i) {
        GLOBAL_SCN_MNGR->destroyLight(mLights[i]);
    }
    mLights.clear();
}
