/*
 * LightManager.h
 *
 *  Created on: 01/05/2012
 *      Author: santiago
 */

#ifndef LMANAGER_H_
#define LMANAGER_H_

//
#define DAYTIME Ogre::ColourValue
#define SUNPOS Ogre::Vector3
#define T_LIGHT Ogre::ColourValue
//

// DAYLIGHT values
#define MIDNIGHT	DAYTIME(0.35f,0.35f,0.45f)
#define P_MIDNIGHT	SUNPOS(0,-1,.35)

#define DAWN		DAYTIME(1.0f,0.75f,.65f)
#define P_DAWN		SUNPOS(.65,-1,.35)

#define NOON		DAYTIME(1.0f,1.0f,0.95f)
#define P_NOON		SUNPOS(0,-1,.35)

#define DUSK		DAYTIME(0.75f,0.75f,0.90f)
#define P_DUSK		SUNPOS(-.65,-1,.35)
//

// T_LIGHT values
#define HALOGEN		Ogre::ColourValue(0.35f,0.35f,0.45f)
#define FLUORESCENT Ogre::ColourValue(0.83f,0.92f,1.0f)
#define TUNGSTEN	Ogre::ColourValue(1.0f,0.77f,0.56f)



class LightManager {

	private:
		Ogre::SceneManager* _manager;
		Ogre::SceneNode* _levelNode;
		Ogre::SceneNode* _sunNode;
		Ogre::Light* _sunLight;
		// TODO: vector for static lights
		// TODO: vector for dynamic lights
		float _index;


	public:
		// set the root SceneNode used in the level
		void setScene(Ogre::SceneNode* node);

		// reset the manager, erase all lights created
		void resetLManager(Ogre::SceneManager* manager);

		// enables shadow casting for objects in the manager
		void enableShadows();

		// disables shadow casting for objects in the manager
		void disableShadows();

		/* set the main light caster for this level (sunlight)
		 * daytime values: MIDNIGHT, DAWN, DUSK, NOON
		 */
		void setDaytime(DAYTIME dt, SUNPOS pos);

		/* creates a static point light (lamp, streetlight, etc...)
		 * this light is static: not meant to be moved or changed, use for level enviromentals
		 * note that the position is relative to the main level node
		 * type values: HALOGEN, FLUORESCENT, TUNGSTEN	
		 */
		void createStaticlight(const Ogre::Vector3 &position, T_LIGHT type, float intensity, float range);

		/* creates a mobile point light (lights attached to mobile objects)
		 * this light is attached to 'father' as a child node
		 * note that the position must be relative to the father node
		 * type values: HALOGEN, FLUORESCENT, TUNGSTEN	
		 */
		void createDynamiclight(Ogre::SceneNode* father, const Ogre::Vector3 &position, T_LIGHT type, float intensity, float range);
		
		/* creates a Flashlight
		 * this light is attached to 'father' as a child node
		 * note that the initial direction is always down-forward
		 */
		void createFlashlight(Ogre::SceneNode* father);

		LightManager(Ogre::SceneManager* manager);

		~LightManager();

};

#endif /* LMANAGER_H_ */