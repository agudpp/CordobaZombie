
#include <Ogre.h>;
#include "OIS\OIS.h";
#include "LightManager.h";

void LightManager::setScene(Ogre::SceneNode* node){
	_levelNode = node;
}

void LightManager::resetLManager(Ogre::SceneManager* manager){
	_manager = manager;
	_levelNode = _manager->getRootSceneNode();
	if(_sunNode) delete(_sunNode);
	if(_sunLight) delete(_sunLight);

	_manager->setShadowTechnique(Ogre:: SHADOWTYPE_NONE);
	_manager->setAmbientLight(Ogre::ColourValue(0, 0, 0));
	_sunNode = _manager->createSceneNode("SunNode");
	_sunNode->setPosition(0,100,0);
	_sunLight = _manager->createLight("SunLight");
	_sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
	_sunLight->setDiffuseColour(NOON);
	_sunLight->setDirection(P_NOON);	
	_sunNode->attachObject(_sunLight);

	//TODO: kill all the lights!!!
	_index=0.0; 
}

void LightManager::enableShadows(){
	_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}

void LightManager::disableShadows(){
	_manager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
}

void LightManager::setDaytime(DAYTIME dt, SUNPOS pos){
	_sunLight->setDiffuseColour(dt);
	_sunLight->setDirection(pos);
}

void LightManager::createStaticlight(const Ogre::Vector3 &position, T_LIGHT type, float intensity, float range){
	Ogre::Light* newLight = _manager->createLight(Ogre::StringConverter::toString(_index));
	newLight->setType(Ogre::Light::LT_POINT);
	newLight->setDiffuseColour(type);
	newLight->setPosition(position);
	/*attenuation parameters
	 *	range	The absolute upper range of the light in world units
	 *	constant	The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation
	 *	linear	The linear factor in the attenuation formula: 1 means attenuate evenly over the distance
	 *	quadratic	The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula.
	 */
	newLight->setAttenuation(range,1.0-intensity,0.05,0);
	//marker
	//  /**flag for deletion: only for demo purposes*/
	Ogre::Entity* newEnt = _manager->createEntity(Ogre::StringConverter::toString(_index),"sphere.mesh");
	Ogre::SceneNode* node = _levelNode->createChildSceneNode(Ogre::StringConverter::toString(_index));
	node->setScale(0.02f,0.02f,0.02f);
	node->setPosition(position);
	node->attachObject(newEnt);
	//	/**	*/
	_index++;
}

void LightManager::createDynamiclight(Ogre::SceneNode* father, const Ogre::Vector3 &position, T_LIGHT type, float intensity, float range){
	Ogre::Light* newLight = _manager->createLight(Ogre::StringConverter::toString(_index));
	newLight->setType(Ogre::Light::LT_POINT);
	newLight->setDiffuseColour(type);
	newLight->setPosition(position);
	/*attenuation parameters
	 *	range	The absolute upper range of the light in world units
	 *	constant	The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation
	 *	linear	The linear factor in the attenuation formula: 1 means attenuate evenly over the distance
	 *	quadratic	The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula.
	 */
	newLight->setAttenuation(range,1.0-intensity,0.05,0);
	father->attachObject(newLight);
	//marker
	//  /**flag for deletion: only for demo purposes*/
	Ogre::Entity* newEnt = _manager->createEntity(Ogre::StringConverter::toString(_index)+="LEnt","sphere.mesh");
	Ogre::SceneNode* node = father->createChildSceneNode(Ogre::StringConverter::toString(_index)+="LEnt");
	node->setScale(0.01f,0.01f,0.01f);
	node->setPosition(position);
	node->attachObject(newEnt);
	//	/**	*/
	_index++;
}
		
void LightManager::createFlashlight(Ogre::SceneNode* father){
	Ogre::Light* newLight = _manager->createLight(Ogre::StringConverter::toString(_index));
	newLight->setType(Ogre::Light::LT_SPOTLIGHT);
	newLight->setDiffuseColour(FLUORESCENT);
	newLight->setPosition(Ogre::Vector3(0,0,15));
	newLight->setDirection(Ogre::Vector3(0,-1,8));
	/*attenuation parameters
	 *	range	The absolute upper range of the light in world units
	 *	constant	The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation
	 *	linear	The linear factor in the attenuation formula: 1 means attenuate evenly over the distance
	 *	quadratic	The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula.
	 */
	//TODO: Range for this light should be smaller
	father->attachObject(newLight);
	_index++;
}

LightManager::LightManager(Ogre::SceneManager* manager){
	_manager = manager;
	_levelNode = _manager->getRootSceneNode();
	_index=0.0;
	//no shadows
	_manager->setShadowTechnique(Ogre:: SHADOWTYPE_NONE);
	//lets disable the ambient light...
	_manager->setAmbientLight(Ogre::ColourValue(0, 0, 0));
	//...and create our own
	_sunNode = _manager->createSceneNode("SunNode");
	_sunNode->setPosition(0,100,0);
	_sunLight = _manager->createLight("SunLight");
	_sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
	_sunLight->setDiffuseColour(NOON);
	_sunLight->setDirection(P_NOON);	
	_sunNode->attachObject(_sunLight);
}

LightManager::~LightManager() {
	if(_sunNode) delete(_sunNode);
	if(_sunLight) delete(_sunLight);
	//TODO delete all lights created by ths manager?
}
