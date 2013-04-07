/*
 * Util.cpp
 *
 *  Created on: 08/11/2011
 *      Author: agustin
 */

#include <stdlib.h>

#include "Util.h"


/* Multiplatform auxiliary function */
#if defined(_WIN32) || defined(CYGWIN)
static inline bool
fileExists(std::string fname)
{
	return System::IO::File::Exists(fname);
}
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <unistd.h>
static inline bool
fileExists(std::string fname)
{
	return !access(fname.c_str(), R_OK);
}
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif


namespace Common
{


////////////////////////////////////////////////////////////////////////////////
Ogre::Quaternion Util::parseQuaternion(TiXmlElement *XMLNode)
{
	Ogre::Quaternion orientation;

	if(XMLNode->Attribute("qx"))
	{
		orientation.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("qx"));
		orientation.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("qy"));
		orientation.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("qz"));
		orientation.w = Ogre::StringConverter::parseReal(XMLNode->Attribute("qw"));
	}
	else if(XMLNode->Attribute("axisX"))
	{
		Ogre::Vector3 axis;
		axis.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisX"));
		axis.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisY"));
		axis.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisZ"));
		Ogre::Real angle = Ogre::StringConverter::parseReal(XMLNode->Attribute("angle"));;
		orientation.FromAngleAxis(Ogre::Angle(angle), axis);
	}
	else if(XMLNode->Attribute("angleX"))
	{
		Ogre::Vector3 axis;
		axis.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleX"));
		axis.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleY"));
		axis.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleZ"));
		//orientation.FromAxes(&axis);
		//orientation.F
	}

	return orientation;
}

////////////////////////////////////////////////////////////////////////////////
Ogre::Vector3 Util::parseVector3(TiXmlElement *XMLNode)
{
    return Ogre::Vector3(
    		Ogre::StringConverter::parseReal(XMLNode->Attribute("x")),
    		Ogre::StringConverter::parseReal(XMLNode->Attribute("y")),
    		Ogre::StringConverter::parseReal(XMLNode->Attribute("z"))
    );
}

/* Creates a list of triangles and allocating all the vertexs from a mesh
 * Requires:
 * 	@cont		The Vertex container where it will been put the vertexs
 * 	@mesh		The mesh to extract the triangles
 * 	@triangles	The list to be filled with the triangles
 * Returns:
 * 	true		on success
 * 	false		otherwise
 */
bool Util::getTrianglesFromMesh(PolyStructsContainer<sm::Vertex *> &cont,
		PolyStructsContainer<Triangle *> &triangles,
		Ogre::MeshPtr mesh, const Ogre::Matrix4 &trmatrix)
{
	ASSERT(!mesh.isNull());

	if(!cont.isEmpty()){
		debug("Warning: Not an empty container\n");
	}

	if(!triangles.isEmpty()){
		debug("Warning, triangles is not empty\n");
		ASSERT(false);
	}

	size_t vertex_count,index_count;
	Ogre::Vector3* vertices = 0;
	long unsigned* indices = 0;

	getMeshInformation(mesh.get(),vertex_count,vertices,index_count,indices);

	// Apply parent SceneNode transformations to all vertices of mesh
	if (trmatrix != Ogre::Matrix4::IDENTITY) {
		for (size_t i = 0 ; i < vertex_count ; i++) {
			vertices[i] = trmatrix * vertices[i];
		}
	}

	// TODO: hacer esta funcion mas rapida, estamos buscando para cada vector
	// el vertice asociado. Es lento

	// here we will map Ogre::Vector3[i] -> Vertex*
	std::vector<sm::Vertex *> vertexMap;
	vertexMap.resize(vertex_count);

	// fills the map of vertexs
	sm::Vertex *v = 0;
	for(size_t i = 0; i < vertex_count; ++i){
		v = findVertex(cont.getObjs(), vertices[i]);
		if(!v){
			// create a new vertex and put it in the container
			v = new sm::Vertex(vertices[i].x, vertices[i].z);
			cont.addObj(v);
		}

		// associate the vec to the vertex
		vertexMap[i] = v;
	}

	// now we have to create the triangles
	for(size_t i = 0; i < index_count; i += 3){
		triangles.addObj(new Triangle(vertexMap[indices[i]],
				vertexMap[indices[i+1]],
				vertexMap[indices[i+2]]));
	}

	delete []vertices;
	delete []indices;

	return true;
}


/**
 * Read a environment variable
 * @param	var		The varname
 * @param	result	The result of the environment variable
 * @return 	True on success, false otherwise
 */
bool Util::readEnvVar(const std::string &var, std::string &result)
{
#ifdef _WIN32
	debug("Getting environment varible:  %s\n", var.c_str());
	char env[1000] = "";

	GetEnvironmentVariable(var.c_str(), env, 1000);

	debug("Size of the buffer: %d\n", strlen(env));

	if(strlen(env) == 0){//If the buffer is empty we didn't find the variable
			debug("The env variable %s wasn't found\n", var.c_str());
			system("PAUSE");
			return false;
		}
	// else set the value
	result = env;
	return true;

#else
	// for linux
	char *env = getenv(var.c_str());
	if(env == 0){
		debug("The env variable %s wasn't found\n", var.c_str());
		return false;
	}
	// else set the value
	result = env;
	return true;
#endif
}



// Find a vertex from a vector
sm::Vertex *Util::findVertex(const std::vector<sm::Vertex *> &vertexs,
		const Ogre::Vector3 &vec)
{
	for(int i = vertexs.size()-1; i >= 0; --i){
		if((vertexs[i]->x == vec.x) && (vertexs[i]->y == vec.z)){
			return vertexs[i];
		}
	}
	return 0;
}


/* Function used to load an AnimationState to a sceneNode.
 * Params:
 * 	@scnManager		the SceneManager
 * 	@node			The SceneNode to load the AnimationStates
 * 	@elem			The TiXmlElement where is the animation
 * Returns:
 * 	anim			On success
 * 	0				On error
 */
bool Util::getAnimation(Ogre::SceneManager *scnManager,
		Ogre::SceneNode *node,
		TiXmlElement *elem,
		std::list<Ogre::AnimationState *> &animList)
{
	ASSERT(scnManager);
	ASSERT(node);
	ASSERT(elem);

	if(Ogre::String(elem->Value()) != "animations") {
		debugERROR("Invalid animation xml: %s \n", elem->Value());
		return false;
	}
	animList.clear();

	TiXmlElement *pElement = elem->FirstChildElement("animation");
	if(!pElement){
		debugERROR("No animations found\n");
		return false;
	}
	while(pElement){
		TiXmlElement *actualElement = pElement;
		Ogre::String nombreanimacion = actualElement->Attribute("name");
		Ogre::String activada = actualElement->Attribute("enable");
		Ogre::String loop = actualElement->Attribute("loop");
		Ogre::String modointerpolacion = actualElement->Attribute("interpolationMode");
		Ogre::String modointerpolacionrotacion = actualElement->Attribute("rotationInterpolationMode");
		Ogre::Real longitud= Ogre::StringConverter::parseReal(actualElement->Attribute("length"));

		Ogre::SceneManager *sceneMgr = scnManager;
		Ogre::Animation *animrueda = sceneMgr->createAnimation(nombreanimacion,longitud);

		if (modointerpolacion == "spline") {
			animrueda->setInterpolationMode(Ogre::Animation::IM_SPLINE);
		} else //linear
		{
			animrueda->setInterpolationMode(Ogre::Animation::IM_LINEAR);
		}

		if (modointerpolacionrotacion == "spherical") {
			animrueda->setRotationInterpolationMode(Ogre::Animation::RIM_SPHERICAL);
		} else //linear
		{
			animrueda->setRotationInterpolationMode(Ogre::Animation::RIM_LINEAR);
		}

		Ogre::NodeAnimationTrack *track = animrueda->createNodeTrack(
				animrueda->getNumNodeTracks() + 1, node);

		actualElement = actualElement->FirstChildElement();
		do {
			Ogre::Real tiempo = Ogre::StringConverter::parseReal(
					actualElement->Attribute("time"));
			Ogre::TransformKeyFrame *kf = track->createNodeKeyFrame(tiempo);

			kf->setTranslate(
					parseVector3(actualElement->FirstChildElement("translation")));
			kf->setRotation(
					parseQuaternion(
							actualElement->FirstChildElement("rotation")));
			kf->setScale(
					parseVector3(
							actualElement->FirstChildElement("scale")));

		} while (actualElement = actualElement->NextSiblingElement());

		// Create the animation and put it in the list
		Ogre::AnimationState *as = scnManager->createAnimationState(nombreanimacion);
		as->setEnabled(false);
		as->setLoop(false);
		ASSERT(as);
		animList.push_back(as);
		pElement = pElement->NextSiblingElement("animation");
	}

	return true;
}

Ogre::AnimationState *
Util::getAnimationFromFile(Ogre::SceneManager *scnManager,
                           Ogre::SceneNode *node,
                           const char *xmlFilename)
{
    ASSERT(scnManager != 0);
    ASSERT(node != 0);
    ASSERT(xmlFilename != 0);
    // load the file
    std::auto_ptr<TiXmlDocument> doc(loadXmlDocument(xmlFilename));
    if(!doc.get()){
        debugERROR("Error loading the animation for the camera %s not found\n",
                xmlFilename);
        return 0;
    }
    TiXmlElement *anim = doc->RootElement();
    if (anim == 0 || !(anim = anim->FirstChildElement("animations"))){
        debugERROR("Error: No animation found in the xml %s\n", xmlFilename);
        return 0;
    }

    // Now parse the animations
    std::list<Ogre::AnimationState *> animList;
    if(!getAnimation(scnManager,
                     node,
                     anim,
                     animList)){
        debugERROR("Error: Some error occur when loading the animation in %s\n",
                xmlFilename);
        return 0;
    }

    // tODO podemos optimizar esto solo leyendo la primera animacion en vez de todas
    // si es que el archivo tiene mas de una
    if (animList.empty()) {
        debugERROR("No animations found for %s\n", xmlFilename);
        return 0;
    }
    const Ogre::String &animName = animList.front()->getAnimationName();
    // now we have the anim, check for that
    ASSERT(scnManager->hasAnimationState(animName));

    return scnManager->getAnimationState(animName);;
}


/* Loads an XmlDocument from a filename.
 * Requires:
 * 	@filename		The xml filename
 * Returns:
 * 	@doc			The xmlDocument or 0 if error
 */
TiXmlDocument *Util::loadXmlDocument(const char *fname)
{
	ASSERT(fname);
	TiXmlDocument   *XMLDoc = 0;

	try
	{
		// Strip the path
		Ogre::String basename, path;
		Ogre::StringUtil::splitFilename(fname, basename, path);

		// we will hardcode the groupName
		Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().
			openResource( basename, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

		//DataStreamPtr pStream = ResourceGroupManager::getSingleton().
		//    openResource( SceneName, groupName );

		Ogre::String data = pStream->getAsString();

		// Open the .scene File
		XMLDoc = new TiXmlDocument();
		XMLDoc->Parse( data.c_str() );

		pStream->close();
		pStream.setNull();

		if( XMLDoc->Error() )
		{
			//We'll just log, and continue on gracefully
			debug("Couldn't load the %s xml\n", fname);
			delete XMLDoc;
			return 0;
		}
	}
	catch(...)
	{
		//We'll just log, and continue on gracefully
		debug("Error creating TiXmlDocument for %s\n", fname);
		delete XMLDoc;
		return 0;
	}

	// Close the XML File
	return XMLDoc;
}



/* Removes all the entities of a sceneNode (recursively) */
void Util::removeAllEntities(Ogre::SceneNode *node)
{
	ASSERT(node);

	// Destroy all the attached objects
	Ogre::SceneNode::ObjectIterator itObject =
			node->getAttachedObjectIterator();

	while (itObject.hasMoreElements())
	{
		Ogre::MovableObject* pObject =
				static_cast<Ogre::MovableObject*> (itObject.getNext());
		node->getCreator()->destroyMovableObject(pObject);
	}

	// Recurse to child SceneNodes
	Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

	while (itChild.hasMoreElements())
	{
		Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*> (itChild.getNext());
		removeAllEntities(pChildNode);
	}
}

/* Remove all (entities and childrens scene nodes) */
void Util::removeAllFromNode(Ogre::SceneNode *node)
{
	ASSERT(node);

	removeAllEntities(node);
	if(node->getParentSceneNode()){
		node->getParentSceneNode()->removeAndDestroyChild(node->getName());
	}

}


////////////////////////////////////////////////////////////////////////////////
void Util::getMeshInformation(const Ogre::Mesh* const mesh,
                        size_t &vertex_count,
                        Ogre::Vector3* &vertices,
                        size_t &index_count,
                        unsigned long* &indices,
                        const Ogre::Vector3 &position,
                        const Ogre::Quaternion &orient,
                        const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }
        // Add the indices
        index_count += submesh->indexData->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                                          static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

////////////////////////////////////////////////////////////////////////////////
int Util::getResourcePath( Ogre::String resourceGroup
				         , Ogre::String resourceName
						 , Ogre::String &resourcePath
						 )
{
	std::string sNameFullPath;

	/* First find audio file absolute path */
	Ogre::ResourceGroupManager& resGM = Ogre::ResourceGroupManager::getSingleton();
	Ogre::FileInfoListPtr files = resGM.findResourceFileInfo(
			resourceGroup, resourceName);

	if (files.isNull()) {
		debug("%s","Recurso no encontrado.\n");
		return -1;

	} else {
		Ogre::FileInfoList::iterator it;
		for (it = files->begin() ; it < files->end() ; it++) {
			/* Compose audio file absolute path */
			sNameFullPath.append(it->archive->getName()+"/"+resourceName);
			if (fileExists(sNameFullPath)) {
				break;
			} else {
				sNameFullPath.clear();
			}
		}
		/* Found? */
		if (it == files->end() || sNameFullPath.size() <= 0) {
			debug("%s","Recurso no encontrado.\n");
			return -1;		}
	}

	resourcePath = sNameFullPath;

	return 0;
}


}
