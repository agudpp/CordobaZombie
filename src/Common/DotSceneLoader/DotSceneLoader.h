#ifndef DOT_SCENELOADER_H
#define DOT_SCENELOADER_H

// Includes
#include <OgreString.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <vector>

//#include "AxisAlignedRectangle.h"
//#include "RegionGeometryMapper.h"


// Forward declarations
class TiXmlElement;

namespace Ogre
{
    // Forward declarations
    class SceneManager;
    class SceneNode;

    class nodeProperty
    {
    public:
        String nodeName;
        String propertyNm;
        String valueName;
        String typeName;

        nodeProperty(const String &node, const String &propertyName, const String &value, const String &type)
            : nodeName(node), propertyNm(propertyName), valueName(value), typeName(type) {}
    };

    class DotSceneLoader
    {
    public:
        DotSceneLoader() : mSceneMgr(0) {}
        virtual ~DotSceneLoader() {}

        void parseDotScene(const String &SceneName, const String &groupName, SceneManager *yourSceneMgr, SceneNode *pAttachNode = NULL, const String &sPrependNode = "");
        String getProperty(const String &ndNm, const String &prop);

        std::vector<nodeProperty> nodeProperties;
        std::vector<String> staticObjects;
        std::vector<String> dynamicObjects;

        /* 						Parse additional data 						*/

        // TODO: cuando cambiemos RegionGeometryMapper usando SceneNodes, deberiamos
        // modificar aca esto ya que solo estamos soportando StaticGeometry
        //std::list<LevelZone::RegionGeometryMapper>		levelZones;
        std::list<StaticGeometry *> 					staticGeometry;

        /* Loads all the staticGeometry to the scene.
         * Fill the vector of Rectangles (LevelZones) of the .scene
         * Returns the list of builded StaticGeometry.
         *
         * RETURNS:
         * 		true 		on success
         * 		false		on error
         */
        bool parseStaticGeometry(const String &SceneName,
								const String &groupName,
								SceneManager *yourSceneMgr,
								SceneNode *pAttachNode = 0,
								const String &sPrependNode = "");



    public:
        void processScene(TiXmlElement *XMLRoot);

        void processNodes(TiXmlElement *XMLNode, Ogre::SceneNode *parent = 0);
        void processExternals(TiXmlElement *XMLNode);
        void processEnvironment(TiXmlElement *XMLNode);
        void processTerrain(TiXmlElement *XMLNode);
        void processUserDataReference(TiXmlElement *XMLNode, SceneNode *pParent = 0);
        void processUserDataReference(TiXmlElement *XMLNode, Entity *pEntity);
        void processOctree(TiXmlElement *XMLNode);
        void processLight(TiXmlElement *XMLNode, SceneNode *pParent = 0);
        void processCamera(TiXmlElement *XMLNode, SceneNode *pParent = 0);

        void processNode(TiXmlElement *XMLNode, SceneNode *pParent = 0);
        void processLookTarget(TiXmlElement *XMLNode, SceneNode *pParent);
        void processTrackTarget(TiXmlElement *XMLNode, SceneNode *pParent);
        void processEntity(TiXmlElement *XMLNode, SceneNode *pParent);
        void processParticleSystem(TiXmlElement *XMLNode, SceneNode *pParent);
        void processBillboardSet(TiXmlElement *XMLNode, SceneNode *pParent);
        void processPlane(TiXmlElement *XMLNode, SceneNode *pParent);

        void processFog(TiXmlElement *XMLNode);
        void processSkyBox(TiXmlElement *XMLNode);
        void processSkyDome(TiXmlElement *XMLNode);
        void processSkyPlane(TiXmlElement *XMLNode);
        void processClipping(TiXmlElement *XMLNode);

        void processLightRange(TiXmlElement *XMLNode, Light *pLight);
        void processLightAttenuation(TiXmlElement *XMLNode, Light *pLight);

        static String getAttrib(TiXmlElement *XMLNode, const String &parameter, const String &defaultValue = "");
        static Real getAttribReal(TiXmlElement *XMLNode, const String &parameter, Real defaultValue = 0);
        static bool getAttribBool(TiXmlElement *XMLNode, const String &parameter, bool defaultValue = false);
        static int getAttribInt(TiXmlElement *XMLNode, const String &parameter, int defaultValue = 0);

        static Vector3 parseVector3(TiXmlElement *XMLNode);
        static Quaternion parseQuaternion(TiXmlElement *XMLNode);
        static ColourValue parseColour(TiXmlElement *XMLNode);


        // ADDED
        bool processEntityStaticGeoemtry(TiXmlElement *XMLNode, StaticGeometry *sgeo);
        bool processStaticGeometries(TiXmlElement *XMLNode);
        bool processLevelZones(TiXmlElement *XMLNode);



        SceneManager *mSceneMgr;
        SceneNode *mAttachNode;
        String m_sGroupName;
        String m_sPrependNode;
    };
}

#endif // DOT_SCENELOADER_H
