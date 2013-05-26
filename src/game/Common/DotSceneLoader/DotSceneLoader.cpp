#include "DotSceneLoader.h"

#include <sstream>  // std::ostringstream
#include <Ogre.h>

#include "DebugUtil.h"
#include "tinyxml.h"

using namespace std;
namespace Ogre {

void DotSceneLoader::parseDotScene(const String &SceneName,
								   const String &groupName,
								   SceneManager *yourSceneMgr,
								   SceneNode *pAttachNode,
								   const String &sPrependNode)
{
    // set up shared object values
    m_sGroupName = groupName;
    mSceneMgr = yourSceneMgr;
    m_sPrependNode = sPrependNode;
    staticObjects.clear();
    dynamicObjects.clear();

    TiXmlDocument   *XMLDoc = 0;
    TiXmlElement   *XMLRoot;

    try
    {
        // Strip the path
        Ogre::String basename, path;
        Ogre::StringUtil::splitFilename(SceneName, basename, path);

        DataStreamPtr pStream = ResourceGroupManager::getSingleton().
            openResource( basename, groupName );

        //DataStreamPtr pStream = ResourceGroupManager::getSingleton().
        //    openResource( SceneName, groupName );

        String data = pStream->getAsString();

        // Open the .scene File
        XMLDoc = new TiXmlDocument();
        XMLDoc->Parse( data.c_str() );

        pStream->close();
        pStream.setNull();

        if( XMLDoc->Error() )
        {
            //We'll just log, and continue on gracefully
        	std::ostringstream error;
        	error << "[DotSceneLoader:" << __LINE__ << "] "
        		  << "The TiXmlDocument reported an error";
            LogManager::getSingleton().logMessage(error.str().c_str());
            delete XMLDoc;
            return;
        }
    }
    catch(Ogre::Exception &e)
    {
        //We'll just log, and continue on gracefully
    	std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error creating TiXmlDocument\n" << e.what();
        LogManager::getSingleton().logMessage(error.str().c_str());
        delete XMLDoc;
        return;
    }

    // Validate the File
    XMLRoot = XMLDoc->RootElement();
    if( String( XMLRoot->Value()) != "scene"  ) {
    	std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error: Invalid .scene File. Missing <scene>";
        LogManager::getSingleton().logMessage(error.str().c_str());
        delete XMLDoc;
        return;
    }

    // figure out where to attach any nodes we create
    mAttachNode = pAttachNode;
    if(!mAttachNode)
        mAttachNode = mSceneMgr->getRootSceneNode();


    // Process the scene
    processScene(XMLRoot);

    // Close the XML File
    delete XMLDoc;
}


void DotSceneLoader::processScene(TiXmlElement *XMLRoot)
{
    // Process the scene parameters
    String version = getAttrib(XMLRoot, "formatVersion", "unknown");

    String message = "[DotSceneLoader] Parsing dotScene file with version " + version;
    if(XMLRoot->Attribute("ID"))
        message += ", id " + String(XMLRoot->Attribute("ID"));
    if(XMLRoot->Attribute("sceneManager"))
        message += ", scene manager " + String(XMLRoot->Attribute("sceneManager"));
    if(XMLRoot->Attribute("minOgreVersion"))
        message += ", min. Ogre version " + String(XMLRoot->Attribute("minOgreVersion"));
    if(XMLRoot->Attribute("author"))
        message += ", author " + String(XMLRoot->Attribute("author"));

    LogManager::getSingleton().logMessage(message);

    TiXmlElement *pElement;

    // Process nodes (?)
    pElement = XMLRoot->FirstChildElement("nodes");
    if(pElement)
        processNodes(pElement);

    // Process externals (?)
    pElement = XMLRoot->FirstChildElement("externals");
    if(pElement)
        processExternals(pElement);

    // Process environment (?)
    pElement = XMLRoot->FirstChildElement("environment");
    if(pElement)
        processEnvironment(pElement);

    // Process terrain (?)
    pElement = XMLRoot->FirstChildElement("terrain");
    if(pElement)
        processTerrain(pElement);

    // Process userDataReference (?)
    pElement = XMLRoot->FirstChildElement("userDataReference");
    if(pElement)
        processUserDataReference(pElement);

    // Process octree (?)
    pElement = XMLRoot->FirstChildElement("octree");
    if(pElement)
        processOctree(pElement);

    // Process light (?)
    pElement = XMLRoot->FirstChildElement("light");
    if(pElement)
        processLight(pElement);

    // Process camera (?)
    pElement = XMLRoot->FirstChildElement("camera");
    if(pElement)
        processCamera(pElement);
}

void DotSceneLoader::processNodes(TiXmlElement *XMLNode, Ogre::SceneNode *parent)
{
    TiXmlElement *pElement;

    if(parent){
    	mAttachNode = parent;
    }
    // Process node (*)
    pElement = XMLNode->FirstChildElement("node");
    while(pElement)
    {
        processNode(pElement);
        pElement = pElement->NextSiblingElement("node");
    }

    // Process position (?)
    pElement = XMLNode->FirstChildElement("position");
    if(pElement)
    {
        mAttachNode->setPosition(parseVector3(pElement));
        mAttachNode->setInitialState();
    }

    // Process rotation (?)
    pElement = XMLNode->FirstChildElement("rotation");
    if(pElement)
    {
        mAttachNode->setOrientation(parseQuaternion(pElement));
        mAttachNode->setInitialState();
    }

    // Process scale (?)
    pElement = XMLNode->FirstChildElement("scale");
    if(pElement)
    {
        mAttachNode->setScale(parseVector3(pElement));
        mAttachNode->setInitialState();
    }
}

void DotSceneLoader::processExternals(TiXmlElement *XMLNode)
{
    //! @todo Implement this
}

void DotSceneLoader::processEnvironment(TiXmlElement *XMLNode)
{
    TiXmlElement *pElement;

    // Process fog (?)
    pElement = XMLNode->FirstChildElement("fog");
    if(pElement)
        processFog(pElement);

    // Process skyBox (?)
    pElement = XMLNode->FirstChildElement("skyBox");
    if(pElement)
        processSkyBox(pElement);

    // Process skyDome (?)
    pElement = XMLNode->FirstChildElement("skyDome");
    if(pElement)
        processSkyDome(pElement);

    // Process skyPlane (?)
    pElement = XMLNode->FirstChildElement("skyPlane");
    if(pElement)
        processSkyPlane(pElement);

    // Process clipping (?)
    pElement = XMLNode->FirstChildElement("clipping");
    if(pElement)
        processClipping(pElement);

    // Process colourAmbient (?)
    pElement = XMLNode->FirstChildElement("colourAmbient");
    if(pElement)
        mSceneMgr->setAmbientLight(parseColour(pElement));

    // Process colourBackground (?)
    //! @todo Set the background colour of all viewports (RenderWindow has to be provided then)
    pElement = XMLNode->FirstChildElement("colourBackground");
    if(pElement)
        ;//mSceneMgr->set(parseColour(pElement));

    // Process userDataReference (?)
    pElement = XMLNode->FirstChildElement("userDataReference");
    if(pElement)
        processUserDataReference(pElement);
}

void DotSceneLoader::processTerrain(TiXmlElement *XMLNode)
{
    //! @todo Implement this
}

void DotSceneLoader::processUserDataReference(TiXmlElement *XMLNode, SceneNode *pParent)
{
    //! @todo Implement this
}

void DotSceneLoader::processOctree(TiXmlElement *XMLNode)
{
    //! @todo Implement this
}

void DotSceneLoader::processLight(TiXmlElement *XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");

    // Create the light
    Light *pLight = mSceneMgr->createLight(name);
    if(pParent)
        pParent->attachObject(pLight);

    String sValue = getAttrib(XMLNode, "type");
    if(sValue == "point")
        pLight->setType(Light::LT_POINT);
    else if(sValue == "directional")
        pLight->setType(Light::LT_DIRECTIONAL);
    else if(sValue == "spot")
        pLight->setType(Light::LT_SPOTLIGHT);
    else if(sValue == "radPoint")
        pLight->setType(Light::LT_POINT);

    pLight->setVisible(getAttribBool(XMLNode, "visible", true));
    pLight->setCastShadows(getAttribBool(XMLNode, "castShadows", true));

    TiXmlElement *pElement;

    // Process position (?)
    pElement = XMLNode->FirstChildElement("position");
    if(pElement)
        pLight->setPosition(parseVector3(pElement));

    // Process normal (?)
    pElement = XMLNode->FirstChildElement("normal");
    if(pElement)
        pLight->setDirection(parseVector3(pElement));

    // Process colourDiffuse (?)
    pElement = XMLNode->FirstChildElement("colourDiffuse");
    if(pElement)
        pLight->setDiffuseColour(parseColour(pElement));

    // Process colourSpecular (?)
    pElement = XMLNode->FirstChildElement("colourSpecular");
    if(pElement)
        pLight->setSpecularColour(parseColour(pElement));

    // Process lightRange (?)
    pElement = XMLNode->FirstChildElement("lightRange");
    if(pElement)
        processLightRange(pElement, pLight);

    // Process lightAttenuation (?)
    pElement = XMLNode->FirstChildElement("lightAttenuation");
    if(pElement)
        processLightAttenuation(pElement, pLight);

    // Process userDataReference (?)
    pElement = XMLNode->FirstChildElement("userDataReference");
    if(pElement)
        ;//processUserDataReference(pElement, pLight);
}

void DotSceneLoader::processCamera(TiXmlElement *XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");
    Real fov = getAttribReal(XMLNode, "fov", 45);
    Real aspectRatio = getAttribReal(XMLNode, "aspectRatio", 1.3333);
    String projectionType = getAttrib(XMLNode, "projectionType", "perspective");

    // Create the camera
    Camera *pCamera = mSceneMgr->createCamera(name);
    if(pParent)
        pParent->attachObject(pCamera);

    // Set the field-of-view
    //! @todo Is this always in degrees?
    //pCamera->setFOVy(Ogre::Degree(fov));

    // Set the aspect ratio
    pCamera->setAspectRatio(aspectRatio);

    // Set the projection type
    if(projectionType == "perspective")
        pCamera->setProjectionType(PT_PERSPECTIVE);
    else if(projectionType == "orthographic")
        pCamera->setProjectionType(PT_ORTHOGRAPHIC);

    TiXmlElement *pElement;

    // Process clipping (?)
    pElement = XMLNode->FirstChildElement("clipping");
    if(pElement)
    {
        Real nearDist = getAttribReal(pElement, "near");
        pCamera->setNearClipDistance(nearDist);

        Real farDist =  getAttribReal(pElement, "far");
        pCamera->setFarClipDistance(farDist);
    }

    // Process position (?)
    pElement = XMLNode->FirstChildElement("position");
    if(pElement)
        pCamera->setPosition(parseVector3(pElement));

    // Process rotation (?)
    pElement = XMLNode->FirstChildElement("rotation");
    if(pElement)
        pCamera->setOrientation(parseQuaternion(pElement));

    // Process normal (?)
    pElement = XMLNode->FirstChildElement("normal");
    if(pElement)
        ;//!< @todo What to do with this element?

    // Process lookTarget (?)
    pElement = XMLNode->FirstChildElement("lookTarget");
    if(pElement)
        ;//!< @todo Implement the camera look target

    // Process trackTarget (?)
    pElement = XMLNode->FirstChildElement("trackTarget");
    if(pElement)
        ;//!< @todo Implement the camera track target

    // Process userDataReference (?)
    pElement = XMLNode->FirstChildElement("userDataReference");
    if(pElement)
        ;//!< @todo Implement the camera user data reference
}


void DotSceneLoader::processNode(TiXmlElement *XMLNode, SceneNode *pParent)
{
	// Construct the node's name
	String name = m_sPrependNode + getAttrib(XMLNode, "name");

	// Create the scene node
	SceneNode *pNode;

	if(name.empty())
	{
	  // Let Ogre choose the name
	  if(pParent)
		 pNode = pParent->createChildSceneNode();
	  else
		 pNode = mAttachNode->createChildSceneNode();
	}
	else
	{
	  // Provide the name
	  if(pParent)
		 pNode = pParent->createChildSceneNode(name);
	  else
		 pNode = mAttachNode->createChildSceneNode(name);
	}


	// Process other attributes
	String id = getAttrib(XMLNode, "id");
	bool isTarget = getAttribBool(XMLNode, "isTarget");

	TiXmlElement *pElement;

	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
	{
	  pNode->setPosition(parseVector3(pElement));
	  pNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
	{
	  pNode->setOrientation(parseQuaternion(pElement));
	  pNode->setInitialState();
	}

	// Process scale (?)
	pElement = XMLNode->FirstChildElement("scale");
	if(pElement)
	{
	  pNode->setScale(parseVector3(pElement));
	  pNode->setInitialState();
	}

	pElement = XMLNode->FirstChildElement("animations");
	if(pElement)
	{
		pElement=pElement->FirstChildElement();
		String nombreanimacion=pElement->Attribute("name");
		String activada=pElement->Attribute("enable");
		String loop=pElement->Attribute("loop");
		String modointerpolacion=pElement->Attribute("interpolationMode");
		String modointerpolacionrotacion=pElement->Attribute("rotationInterpolationMode");
		Real longitud=StringConverter::parseReal(pElement->Attribute("length"));

		SceneManager *sceneMgr = mSceneMgr;
		Animation *animrueda = sceneMgr->createAnimation(nombreanimacion,longitud);

		if(modointerpolacion=="spline")
		{
		 animrueda->setInterpolationMode(Animation::IM_SPLINE);
		}
		else  //linear
		{
		 animrueda->setInterpolationMode(Animation::IM_LINEAR);
		}

		if(modointerpolacionrotacion=="spherical")
		{
		 animrueda->setRotationInterpolationMode(Animation::RIM_SPHERICAL);
		}
		else  //linear
		{
		 animrueda->setRotationInterpolationMode(Animation::RIM_LINEAR);
		}

		NodeAnimationTrack *track =animrueda->createNodeTrack(animrueda->getNumNodeTracks() + 1 ,pNode);

		pElement=pElement->FirstChildElement();
		do
		{
			Real tiempo=StringConverter::parseReal(pElement->Attribute("time"));
			TransformKeyFrame *kf=track->createNodeKeyFrame(tiempo);

			kf->setTranslate(this->parseVector3(pElement->FirstChildElement("translation")));
			kf->setRotation(this->parseQuaternion(pElement->FirstChildElement("rotation")));
			kf->setScale(this->parseVector3(pElement->FirstChildElement("scale")));

		}while(pElement=pElement->NextSiblingElement());

		// Creamos el AnimationState dentro del SceneManager para poder sacarlo
		// despues
		mSceneMgr->createAnimationState(nombreanimacion);

	}

	// Process lookTarget (?)
	pElement = XMLNode->FirstChildElement("lookTarget");
	if(pElement)
	  processLookTarget(pElement, pNode);

	// Process trackTarget (?)
	pElement = XMLNode->FirstChildElement("trackTarget");
	if(pElement)
	  processTrackTarget(pElement, pNode);

	// Process node (*)
	pElement = XMLNode->FirstChildElement("node");
	while(pElement)
	{
	  processNode(pElement, pNode);
	  pElement = pElement->NextSiblingElement("node");
	}

	// Process entity (*)
	pElement = XMLNode->FirstChildElement("entity");
	while(pElement)
	{
	  processEntity(pElement, pNode);
	  pElement = pElement->NextSiblingElement("entity");

	}

	// Process light (*)
	pElement = XMLNode->FirstChildElement("light");
	while(pElement)
	{
	  processLight(pElement, pNode);
	  pElement = pElement->NextSiblingElement("light");
	}

	// Process camera (*)
	pElement = XMLNode->FirstChildElement("camera");
	while(pElement)
	{
	  processCamera(pElement, pNode);
	  pElement = pElement->NextSiblingElement("camera");
	}

	// Process particleSystem (*)
	pElement = XMLNode->FirstChildElement("particleSystem");
	while(pElement)
	{
	  processParticleSystem(pElement, pNode);
	  pElement = pElement->NextSiblingElement("particleSystem");
	}

	// Process billboardSet (*)
	pElement = XMLNode->FirstChildElement("billboardSet");
	while(pElement)
	{
	  processBillboardSet(pElement, pNode);
	  pElement = pElement->NextSiblingElement("billboardSet");
	}

	// Process plane (*)
	pElement = XMLNode->FirstChildElement("plane");
	while(pElement)
	{
	  processPlane(pElement, pNode);
	  pElement = pElement->NextSiblingElement("plane");
	}

	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	if(pElement)
	  processUserDataReference(pElement, pNode);
}

void DotSceneLoader::processLookTarget(TiXmlElement *XMLNode, SceneNode *pParent)
{
    //! @todo Is this correct? Cause I don't have a clue actually

    // Process attributes
    String nodeName = getAttrib(XMLNode, "nodeName");

    Node::TransformSpace relativeTo = Node::TS_PARENT;
    String sValue = getAttrib(XMLNode, "relativeTo");
    if(sValue == "local")
        relativeTo = Node::TS_LOCAL;
    else if(sValue == "parent")
        relativeTo = Node::TS_PARENT;
    else if(sValue == "world")
        relativeTo = Node::TS_WORLD;

    TiXmlElement *pElement;

    // Process position (?)
    Vector3 position;
    pElement = XMLNode->FirstChildElement("position");
    if(pElement)
        position = parseVector3(pElement);

    // Process localDirection (?)
    Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
    pElement = XMLNode->FirstChildElement("localDirection");
    if(pElement)
        localDirection = parseVector3(pElement);

    // Setup the look target
    try
    {
        if(!nodeName.empty())
        {
            SceneNode *pLookNode = mSceneMgr->getSceneNode(nodeName);
            position = pLookNode->_getDerivedPosition();
        }

        pParent->lookAt(position, relativeTo, localDirection);
    }
    catch(Ogre::Exception &e)
    {
    	std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error processing a look target!\n" << e.what();
        LogManager::getSingleton().logMessage(error.str().c_str());
    }
}

void DotSceneLoader::processTrackTarget(TiXmlElement *XMLNode, SceneNode *pParent)
{
    // Process attributes
    String nodeName = getAttrib(XMLNode, "nodeName");

    TiXmlElement *pElement;

    // Process localDirection (?)
    Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;
    pElement = XMLNode->FirstChildElement("localDirection");
    if(pElement)
        localDirection = parseVector3(pElement);

    // Process offset (?)
    Vector3 offset = Vector3::ZERO;
    pElement = XMLNode->FirstChildElement("offset");
    if(pElement)
        offset = parseVector3(pElement);

    // Setup the track target
    try
    {
        SceneNode *pTrackNode = mSceneMgr->getSceneNode(nodeName);
        pParent->setAutoTracking(true, pTrackNode, localDirection, offset);
    }
    catch(Ogre::Exception &e)
    {
    	std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error processing a track target!\n" << e.what();
        LogManager::getSingleton().logMessage(error.str().c_str());
    }
}

void DotSceneLoader::processEntity(TiXmlElement *XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");
    String meshFile = getAttrib(XMLNode, "meshFile");
    String materialFile = getAttrib(XMLNode, "materialFile");
    bool isStatic = getAttribBool(XMLNode, "static", false);;
    bool castShadows = getAttribBool(XMLNode, "castShadows", true);

    // TEMP: Maintain a list of static and dynamic objects
    if(isStatic)
        staticObjects.push_back(name);
    else
        dynamicObjects.push_back(name);

    TiXmlElement *pElement;

    // Process vertexBuffer (?)
    pElement = XMLNode->FirstChildElement("vertexBuffer");
    if(pElement) {
        ;//processVertexBuffer(pElement);
    }

    // Process indexBuffer (?)
    pElement = XMLNode->FirstChildElement("indexBuffer");
    if(pElement) {
        ;//processIndexBuffer(pElement);
    }

    // Create the entity
    Entity *pEntity = 0;
    try
    {
        MeshManager::getSingleton().load(meshFile, m_sGroupName);
        pEntity = mSceneMgr->createEntity(name, meshFile);
        pEntity->setCastShadows(castShadows);
        pParent->attachObject(pEntity);

        if(!materialFile.empty()){
        	MaterialPtr mat = MaterialManager::getSingleton().load(materialFile,
        			ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
        	pEntity->setMaterial(mat);

        }
    }catch(Ogre::Exception &e)
    {
    	std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error loading an entity!\n" << e.what();
        LogManager::getSingleton().logMessage(error.str().c_str());
    }

    // Process userDataReference (?)
    pElement = XMLNode->FirstChildElement("userDataReference");
    if(pElement)
        processUserDataReference(pElement, pEntity);


}

void DotSceneLoader::processParticleSystem(TiXmlElement *XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");
    String file = getAttrib(XMLNode, "file");

    // Create the particle system
    try
    {
        ParticleSystem *pParticles = mSceneMgr->createParticleSystem(name, file);
        pParent->attachObject(pParticles);
    }
    catch(Ogre::Exception &e)
    {
    	std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error creating a particle system!\n" << e.what();
        LogManager::getSingleton().logMessage(error.str().c_str());
    }
}

void DotSceneLoader::processBillboardSet(TiXmlElement *XMLNode, SceneNode *pParent)
{
    //! @todo Implement this
}

void DotSceneLoader::processPlane(TiXmlElement *XMLNode, SceneNode *pParent)
{
    //! @todo Implement this
}

void DotSceneLoader::processFog(TiXmlElement *XMLNode)
{
    // Process attributes
    Real expDensity = getAttribReal(XMLNode, "expDensity", 0.001);
    Real linearStart = getAttribReal(XMLNode, "linearStart", 0.0);
    Real linearEnd = getAttribReal(XMLNode, "linearEnd", 1.0);

    FogMode mode = FOG_NONE;
    String sMode = getAttrib(XMLNode, "mode");
    if(sMode == "none")
        mode = FOG_NONE;
    else if(sMode == "exp")
        mode = FOG_EXP;
    else if(sMode == "exp2")
        mode = FOG_EXP2;
    else if(sMode == "linear")
        mode = FOG_LINEAR;

    TiXmlElement *pElement;

    // Process colourDiffuse (?)
    ColourValue colourDiffuse = ColourValue::White;
    pElement = XMLNode->FirstChildElement("colourDiffuse");
    if(pElement)
        colourDiffuse = parseColour(pElement);

    // Setup the fog
    mSceneMgr->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}

void DotSceneLoader::processSkyBox(TiXmlElement *XMLNode)
{
    // Process attributes
    String material = getAttrib(XMLNode, "material");
    Real distance = getAttribReal(XMLNode, "distance", 5000);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    TiXmlElement *pElement;

    // Process rotation (?)
    Quaternion rotation = Quaternion::IDENTITY;
    pElement = XMLNode->FirstChildElement("rotation");
    if(pElement)
        rotation = parseQuaternion(pElement);

    // Setup the sky box
    mSceneMgr->setSkyBox(true, material, distance, drawFirst, rotation, m_sGroupName);
}

void DotSceneLoader::processSkyDome(TiXmlElement *XMLNode)
{
    // Process attributes
    String material = XMLNode->Attribute("material");
    Real curvature = getAttribReal(XMLNode, "curvature", 10);
    Real tiling = getAttribReal(XMLNode, "tiling", 8);
    Real distance = getAttribReal(XMLNode, "distance", 4000);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    TiXmlElement *pElement;

    // Process rotation (?)
    Quaternion rotation = Quaternion::IDENTITY;
    pElement = XMLNode->FirstChildElement("rotation");
    if(pElement)
        rotation = parseQuaternion(pElement);

    // Setup the sky dome
    mSceneMgr->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, m_sGroupName);
}

void DotSceneLoader::processSkyPlane(TiXmlElement *XMLNode)
{
    // Process attributes
    String material = getAttrib(XMLNode, "material");
    Real planeX = getAttribReal(XMLNode, "planeX", 0);
    Real planeY = getAttribReal(XMLNode, "planeY", -1);
    Real planeZ = getAttribReal(XMLNode, "planeX", 0);
    Real planeD = getAttribReal(XMLNode, "planeD", 5000);
    Real scale = getAttribReal(XMLNode, "scale", 1000);
    Real bow = getAttribReal(XMLNode, "bow", 0);
    Real tiling = getAttribReal(XMLNode, "tiling", 10);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    // Setup the sky plane
    Plane plane;
    plane.normal = Vector3(planeX, planeY, planeZ);
    plane.d = planeD;
    mSceneMgr->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, m_sGroupName);
}

void DotSceneLoader::processClipping(TiXmlElement *XMLNode)
{
    //! @todo Implement this

    // Process attributes
    Real fNear = getAttribReal(XMLNode, "near", 0);
    Real fFar = getAttribReal(XMLNode, "far", 1);
}

void DotSceneLoader::processLightRange(TiXmlElement *XMLNode, Light *pLight)
{
    // Process attributes
    Real inner = getAttribReal(XMLNode, "inner");
    Real outer = getAttribReal(XMLNode, "outer");
    Real falloff = getAttribReal(XMLNode, "falloff", 1.0);

    // Setup the light range
    pLight->setSpotlightRange(Angle(inner), Angle(outer), falloff);
}

void DotSceneLoader::processLightAttenuation(TiXmlElement *XMLNode, Light *pLight)
{
    // Process attributes
    Real range = getAttribReal(XMLNode, "range");
    Real constant = getAttribReal(XMLNode, "constant");
    Real linear = getAttribReal(XMLNode, "linear");
    Real quadratic = getAttribReal(XMLNode, "quadratic");

    // Setup the light attenuation
    pLight->setAttenuation(range, constant, linear, quadratic);
}

String DotSceneLoader::getAttrib(TiXmlElement *XMLNode, const String &attrib, const String &defaultValue)
{
    if(XMLNode->Attribute(attrib.c_str()))
        return XMLNode->Attribute(attrib.c_str());
    else
        return defaultValue;
}

Real DotSceneLoader::getAttribReal(TiXmlElement *XMLNode, const String &attrib, Real defaultValue)
{
    if(XMLNode->Attribute(attrib.c_str()))
        return StringConverter::parseReal(XMLNode->Attribute(attrib.c_str()));
    else
        return defaultValue;
}

bool DotSceneLoader::getAttribBool(TiXmlElement *XMLNode, const String &attrib, bool defaultValue)
{
    if(!XMLNode->Attribute(attrib.c_str()))
        return defaultValue;

    if(String(XMLNode->Attribute(attrib.c_str())) == "true")
        return true;

    return false;
}

int DotSceneLoader::getAttribInt(TiXmlElement *XMLNode, const String &attrib, int defaultValue)
{
	if(XMLNode->Attribute(attrib.c_str()))
		return StringConverter::parseInt(XMLNode->Attribute(attrib.c_str()));
	else
		return defaultValue;
}

Vector3 DotSceneLoader::parseVector3(TiXmlElement *XMLNode)
{
    return Vector3(
        StringConverter::parseReal(XMLNode->Attribute("x")),
        StringConverter::parseReal(XMLNode->Attribute("y")),
        StringConverter::parseReal(XMLNode->Attribute("z"))
    );
}

Quaternion DotSceneLoader::parseQuaternion(TiXmlElement *XMLNode)
{
    //! @todo Fix this crap!

    Quaternion orientation;

    if(XMLNode->Attribute("qx"))
    {
        orientation.x = StringConverter::parseReal(XMLNode->Attribute("qx"));
        orientation.y = StringConverter::parseReal(XMLNode->Attribute("qy"));
        orientation.z = StringConverter::parseReal(XMLNode->Attribute("qz"));
        orientation.w = StringConverter::parseReal(XMLNode->Attribute("qw"));
    }
    else if(XMLNode->Attribute("axisX"))
    {
        Vector3 axis;
        axis.x = StringConverter::parseReal(XMLNode->Attribute("axisX"));
        axis.y = StringConverter::parseReal(XMLNode->Attribute("axisY"));
        axis.z = StringConverter::parseReal(XMLNode->Attribute("axisZ"));
        Real angle = StringConverter::parseReal(XMLNode->Attribute("angle"));;
        orientation.FromAngleAxis(Ogre::Angle(angle), axis);
    }
    else if(XMLNode->Attribute("angleX"))
    {
        Vector3 axis;
        axis.x = StringConverter::parseReal(XMLNode->Attribute("angleX"));
        axis.y = StringConverter::parseReal(XMLNode->Attribute("angleY"));
        axis.z = StringConverter::parseReal(XMLNode->Attribute("angleZ"));
        //orientation.FromAxes(&axis);
        //orientation.F
    }

    return orientation;
}

ColourValue DotSceneLoader::parseColour(TiXmlElement *XMLNode)
{
    return ColourValue(
        StringConverter::parseReal(XMLNode->Attribute("r")),
        StringConverter::parseReal(XMLNode->Attribute("g")),
        StringConverter::parseReal(XMLNode->Attribute("b")),
        XMLNode->Attribute("a") != NULL ? StringConverter::parseReal(XMLNode->Attribute("a")) : 1
    );
}

String DotSceneLoader::getProperty(const String &ndNm, const String &prop)
{
    for ( unsigned int i = 0 ; i < nodeProperties.size(); i++ )
    {
        if ( nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop )
        {
            return nodeProperties[i].valueName;
        }
    }

    return "";
}

void DotSceneLoader::processUserDataReference(TiXmlElement *XMLNode, Entity *pEntity)
{
    ASSERT(false); // que es este user any aca???? no lo debeiramos usar
    // porque puede colisionar con los any que le ponemos nosotros
    String str = XMLNode->Attribute("id");
    pEntity->setUserAny(Any(str));
}



bool  DotSceneLoader::parseStaticGeometry(const String &SceneName,
						const String &groupName,
     					SceneManager *yourSceneMgr,
     					SceneNode *pAttachNode,
     					const String &sPrependNode)
{
	// set up shared object values
	m_sGroupName = groupName;
	mSceneMgr = yourSceneMgr;
	m_sPrependNode = sPrependNode;
	staticObjects.clear();
	dynamicObjects.clear();

	TiXmlDocument   *XMLDoc = 0;
	TiXmlElement   *XMLRoot;

	try
	{
		// Strip the path
		Ogre::String basename, path;
		Ogre::StringUtil::splitFilename(SceneName, basename, path);

		DataStreamPtr pStream = ResourceGroupManager::getSingleton().
			openResource( basename, groupName );

		//DataStreamPtr pStream = ResourceGroupManager::getSingleton().
		//    openResource( SceneName, groupName );

		String data = pStream->getAsString();

		// Open the .scene File
		XMLDoc = new TiXmlDocument();
		XMLDoc->Parse( data.c_str() );

		pStream->close();
		pStream.setNull();

		if( XMLDoc->Error() )
		{
			//We'll just log, and continue on gracefully
	    	std::ostringstream error;
	    	error << "[DotSceneLoader:" << __LINE__ << "] "
	    		  << "The TiXmlDocument reported an error";
			LogManager::getSingleton().logMessage(error.str().c_str());
			delete XMLDoc;
			return false;
		}
	}
	catch(Ogre::Exception &e)
	{
		//We'll just log, and continue on gracefully
    	std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error creating TiXmlDocument\n" << e.what();
		LogManager::getSingleton().logMessage(error.str().c_str());
		delete XMLDoc;
		return false;
	}

	// Validate the File
	XMLRoot = XMLDoc->RootElement();
	if( String( XMLRoot->Value()) != "scene"  ) {
		std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Invalid .scene File. Missing <scene> tag";
		LogManager::getSingleton().logMessage(error.str().c_str());
		delete XMLDoc;
		return false;
	}

	// figure out where to attach any nodes we create
	mAttachNode = pAttachNode;
	if(!mAttachNode)
		mAttachNode = mSceneMgr->getRootSceneNode();


	// Process the scene
	if(!processStaticGeometries(XMLRoot)){
		std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error parsing the StaticGemoetries";
		LogManager::getSingleton().logMessage(error.str().c_str());
		delete XMLDoc;
		//TODO: destroy every staticGeometry
		return false;
	}

	/*if(!processLevelZones(XMLRoot)){
		LogManager::getSingleton().logMessage( "[DotSceneLoader] Error parsing the LevelZones" );
		delete XMLDoc;
		//TODO: destroy every staticGeometry
		return false;
	}*/

	// Close the XML File
	delete XMLDoc;

	return true;
}

bool DotSceneLoader::processEntityStaticGeoemtry(TiXmlElement *XMLNode, StaticGeometry *sgeo)
{
	// Process attributes
	String meshFile = getAttrib(XMLNode, "meshFile");
	Vector3 position = Vector3::ZERO;
	Quaternion rotation = Quaternion::IDENTITY;
	Vector3 scale = Vector3::UNIT_SCALE;
	std::vector<String> subEntities;


	// parse the entity

	// get the origin
	TiXmlElement *auxPElem = XMLNode->FirstChildElement("position");
	if(auxPElem) {
		position = parseVector3(auxPElem);
	}

	// get the rotation
	auxPElem = XMLNode->FirstChildElement("rotation");
	if(auxPElem) {
		rotation = parseQuaternion(auxPElem);
	}

	// get the scale
	auxPElem = XMLNode->FirstChildElement("scale");
	if(auxPElem) {
		scale = parseVector3(auxPElem);
	}

	size_t count = 0;
	// load the subentities
	TiXmlElement *pElement = XMLNode->FirstChildElement("subentities");
	if(pElement){
		auxPElem = pElement->FirstChildElement("subentity");
		// ugly! very
		while(auxPElem) {
			auxPElem = auxPElem->NextSiblingElement("subentity");
			count++;
		}
		subEntities.resize(count);
		pElement = pElement->FirstChildElement("subentity");
		while(pElement){
			int index = getAttribInt(pElement, "index");

			subEntities[index] = getAttrib(pElement, "materialName");

			pElement = pElement->NextSiblingElement("subentity");
		}
	}

	// creates the entity
	Entity *ent = 0;
	try {
		ent = mSceneMgr->createEntity("entity_" + meshFile, meshFile);
	} catch (...) {
		return false;
	}

	// set the subentities config
	 size_t subentityCount = std::min(subEntities.size(), (size_t)ent->getNumSubEntities());
	for (size_t subentityIndex = 0; subentityIndex < subentityCount; subentityIndex++)
	{
		SubEntity* subentity = ent->getSubEntity((unsigned int)subentityIndex);
		subentity->setMaterialName(subEntities[subentityIndex]);
	}

	// add the entitie to the static geometry
	sgeo->addEntity(ent, position, rotation, scale);
	mSceneMgr->destroyEntity(ent);


	return true;
}

bool DotSceneLoader::processStaticGeometries(TiXmlElement *XMLRoot)
{
	 TiXmlElement *pElement, *auxPElem;

	// Process nodes (?)
	pElement = XMLRoot->FirstChildElement("staticGeometries");
	if(!pElement){
		std::ostringstream error;
    	error << "[DotSceneLoader:" << __LINE__ << "] "
    		  << "Error: Invalid .scene processStaticGeometries";
		LogManager::getSingleton().logMessage(error.str().c_str());
		return false;
	}

	staticGeometry.clear();

	StaticGeometry	*sgeo = 0;
	Vector3			origin;
	Vector3			dimension;
	bool			visible;
	String			name;
	bool			castShadow;

	// parse the staticGeometries
	pElement = pElement->FirstChildElement("staticGeometry");
	while(pElement)
	{
		// Parse a StaticGeometry
		visible = getAttribBool(pElement, "visible");
		name = getAttrib(pElement, "name");
		castShadow = getAttribBool(pElement, "castShadows");

		try {
			sgeo = mSceneMgr->createStaticGeometry(name);
		} catch(Ogre::Exception &e) {
			std::ostringstream error;
	    	error << "[DotSceneLoader:" << __LINE__ << "] "
	    		  << "Error: Invalid name to StaticGeometry\n" << e.what();
			LogManager::getSingleton().logMessage(error.str().c_str());
			return false;
		}

		staticGeometry.push_back(sgeo);

		sgeo->setCastShadows(castShadow);
		sgeo->setVisible(visible);

		// get the origin
		auxPElem = pElement->FirstChildElement("origin");
		if(auxPElem) {
			sgeo->setOrigin(parseVector3(auxPElem));
		}

		// get the dimensions
		auxPElem = pElement->FirstChildElement("dimensions");
		if(auxPElem) {
			sgeo->setRegionDimensions(parseVector3(auxPElem));
		}

		// iterate over the entity list
		auxPElem = pElement;
		auxPElem = auxPElem->FirstChildElement("entities");
		auxPElem = auxPElem->FirstChildElement("entity");
		while(auxPElem){
			// get the entities associated
			if(!processEntityStaticGeoemtry(auxPElem, sgeo)){
				std::ostringstream error;
		    	error << "[DotSceneLoader:" << __LINE__ << "] "
		    		  << "Error: Invalid entity of StaticGeometry";
				LogManager::getSingleton().logMessage(error.str().c_str());
				return false;
			}

			auxPElem = auxPElem->NextSiblingElement("entity");
		}

		// build the staticGeometry
		sgeo->build();

		pElement = pElement->NextSiblingElement("staticGeometry");
	}


	return true;

}


/*bool DotSceneLoader::processLevelZones(TiXmlElement *XMLNode)
{
	levelZones.clear();

	TiXmlElement *pElement = XMLNode->FirstChildElement("levelZones");
	if(!pElement){
		LogManager::getSingleton().logMessage( "[DotSceneLoader] Error: "
				"Cannot find levelZones tag in the .scene");
		return false;
	}


	TiXmlElement *auxPElem = 0;
	std::vector<Ogre::StaticGeometry *> sgm;

	pElement = pElement->FirstChildElement("levelZone");
	while(pElement)
	{
		LevelZone::AxisAlignedRectangle rectangle;
		sgm.clear();
		auxPElem = pElement->FirstChildElement("topLeft");
		if(auxPElem) {
			rectangle.mTopLeftX = getAttribReal(auxPElem,"x",0);
			rectangle.mTopLeftY = getAttribReal(auxPElem,"z",0);
		}

		auxPElem = pElement->FirstChildElement("bottomRight");
		if(auxPElem) {
			rectangle.mBottomRightX = getAttribReal(auxPElem,"x",0);
			rectangle.mBottomRightY = getAttribReal(auxPElem,"z",0);
		}

		// get the list of the StaticGeometries
		auxPElem = pElement;
		auxPElem = auxPElem->FirstChildElement("listStaticGeo");
		if(!auxPElem){
			LogManager::getSingleton().logMessage( "[DotSceneLoader] Error: LevelZone without StaticGeometry!" );
			// TODO: free everything here
			return false;
		}

		auxPElem = auxPElem->FirstChildElement("staticGeo");
		while(auxPElem){
			// check that the StaticGeometry already exists
			try{
				sgm.push_back(mSceneMgr->getStaticGeometry(
						getAttrib(auxPElem, "name")));
			} catch(...){
				LogManager::getSingleton().logMessage( "[DotSceneLoader] Error: LevelZone with"
							" an incorrect StaticGeometry Name");
				return false;
			}

			auxPElem = auxPElem->NextSiblingElement("staticGeo");
		}

		LevelZone::RegionGeometryMapper reg(rectangle,sgm);
		levelZones.push_back(reg);

		pElement = pElement->NextSiblingElement("levelZone");
	}

	return true;
}
*/
}
