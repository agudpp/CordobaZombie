/*
 * OgreUtils.cpp
 *
 *  Created on: May 26, 2013
 *      Author: agustin
 */

#include "OgreUtils.h"

#include <stdlib.h>
#include <vector>

#include <Math/CommonMath.h>

// TODO: remove this
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

// Helper methods
//
namespace {

////////////////////////////////////////////////////////////////////////////////
Ogre::Quaternion
parseQuaternion(const TiXmlElement *XMLNode)
{
    Ogre::Quaternion orientation;

    if (XMLNode->Attribute("qx")) {
        orientation.x = Ogre::StringConverter::parseReal(
            XMLNode->Attribute("qx"));
        orientation.y = Ogre::StringConverter::parseReal(
            XMLNode->Attribute("qy"));
        orientation.z = Ogre::StringConverter::parseReal(
            XMLNode->Attribute("qz"));
        orientation.w = Ogre::StringConverter::parseReal(
            XMLNode->Attribute("qw"));
    } else if (XMLNode->Attribute("axisX")) {
        Ogre::Vector3 axis;
        axis.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisX"));
        axis.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisY"));
        axis.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisZ"));
        Ogre::Real angle = Ogre::StringConverter::parseReal(
            XMLNode->Attribute("angle"));
        ;
        orientation.FromAngleAxis(Ogre::Angle(angle), axis);
    } else if (XMLNode->Attribute("angleX")) {
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
Ogre::Vector3
parseVector3(const TiXmlElement *XMLNode)
{
    return Ogre::Vector3(
        Ogre::StringConverter::parseReal(XMLNode->Attribute("x")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("y")),
        Ogre::StringConverter::parseReal(XMLNode->Attribute("z")));
}

////////////////////////////////////////////////////////////////////////////////
core::Vertex*
findVertex(const std::vector<core::Vertex *> &vertexs, const Ogre::Vector3 &vec)
{
    for (int i = vertexs.size() - 1; i >= 0; --i) {
        if ((vertexs[i]->x == vec.x) && (vertexs[i]->y == vec.z)) {
            return vertexs[i];
        }
    }
    return 0;
}
}

namespace game {
namespace OgreUtils {

/* Function used to load an AnimationState to a sceneNode.
 * Params:
 *  @scnManager     the SceneManager
 *  @node           The SceneNode to load the AnimationStates
 *  @elem           The TiXmlElement where is the animation
 * Returns:
 *  anim            On success
 *  0               On error
 */
bool
getAnimation(Ogre::SceneManager *scnManager,
             Ogre::SceneNode *node,
             TiXmlElement *elem,
             std::list<Ogre::AnimationState *> &animList)
{
    ASSERT(scnManager);
    ASSERT(node);
    ASSERT(elem);

    if (Ogre::String(elem->Value()) != "animations") {
        debugERROR("Invalid animation xml: %s \n", elem->Value());
        return false;
    }
    animList.clear();

    TiXmlElement *pElement = elem->FirstChildElement("animation");
    if (!pElement) {
        debugERROR("No animations found\n");
        return false;
    }
    while (pElement) {
        TiXmlElement *actualElement = pElement;
        Ogre::String nombreanimacion = actualElement->Attribute("name");
        Ogre::String activada = actualElement->Attribute("enable");
        Ogre::String loop = actualElement->Attribute("loop");
        Ogre::String modointerpolacion = actualElement->Attribute(
            "interpolationMode");
        Ogre::String modointerpolacionrotacion = actualElement->Attribute(
            "rotationInterpolationMode");
        Ogre::Real longitud = Ogre::StringConverter::parseReal(
            actualElement->Attribute("length"));

        Ogre::SceneManager *sceneMgr = scnManager;
        Ogre::Animation *animrueda = sceneMgr->createAnimation(nombreanimacion,
            longitud);

        if (modointerpolacion == "spline") {
            animrueda->setInterpolationMode(Ogre::Animation::IM_SPLINE);
        } else //linear
        {
            animrueda->setInterpolationMode(Ogre::Animation::IM_LINEAR);
        }

        if (modointerpolacionrotacion == "spherical") {
            animrueda->setRotationInterpolationMode(
                Ogre::Animation::RIM_SPHERICAL);
        } else //linear
        {
            animrueda->setRotationInterpolationMode(
                Ogre::Animation::RIM_LINEAR);
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
                parseQuaternion(actualElement->FirstChildElement("rotation")));
            kf->setScale(
                parseVector3(actualElement->FirstChildElement("scale")));

        } while (actualElement = actualElement->NextSiblingElement());

        // Create the animation and put it in the list
        Ogre::AnimationState *as = scnManager->createAnimationState(
            nombreanimacion);
        as->setEnabled(false);
        as->setLoop(false);
        ASSERT(as);
        animList.push_back(as);
        pElement = pElement->NextSiblingElement("animation");
    }

    return true;
}

Ogre::AnimationState *
getAnimationFromFile(Ogre::SceneManager *scnManager,
                     Ogre::SceneNode *node,
                     const char *xmlFilename)
{
    ASSERT(scnManager != 0);
    ASSERT(node != 0);
    ASSERT(xmlFilename != 0);
    // load the file
    std::auto_ptr<TiXmlDocument> doc(loadXmlDocument(xmlFilename));
    if (!doc.get()) {
        debugERROR("Error loading the animation for the camera %s not found\n",
            xmlFilename);
        return 0;
    }
    TiXmlElement *anim = doc->RootElement();
    if (anim == 0 || !(anim = anim->FirstChildElement("animations"))) {
        debugERROR("Error: No animation found in the xml %s\n", xmlFilename);
        return 0;
    }

    // Now parse the animations
    std::list<Ogre::AnimationState *> animList;
    if (!getAnimation(scnManager, node, anim, animList)) {
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

/* Removes all the entities of a sceneNode (recursively) */
void
removeAllEntities(Ogre::SceneNode *node)
{
    ASSERT(node);

    // Destroy all the attached objects
    Ogre::SceneNode::ObjectIterator itObject =
        node->getAttachedObjectIterator();

    while (itObject.hasMoreElements()) {
        Ogre::MovableObject* pObject =
            static_cast<Ogre::MovableObject*>(itObject.getNext());
        node->getCreator()->destroyMovableObject(pObject);
    }

    // Recurse to child SceneNodes
    Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

    while (itChild.hasMoreElements()) {
        Ogre::SceneNode* pChildNode =
            static_cast<Ogre::SceneNode*>(itChild.getNext());
        removeAllEntities(pChildNode);
    }
}

/* Remove all (entities and childrens scene nodes) */
void
removeAllFromNode(Ogre::SceneNode *node)
{
    ASSERT(node);

    removeAllEntities(node);
    if (node->getParentSceneNode()) {
        node->getParentSceneNode()->removeAndDestroyChild(node->getName());
    }

}

////////////////////////////////////////////////////////////////////////////////
void
getMeshInformation(const Ogre::Mesh* const mesh,
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
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if (submesh->useSharedVertices) {
            if (!added_shared) {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        } else {
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
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data =
            submesh->useSharedVertices ?
                mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices)
            || (submesh->useSharedVertices && !added_shared)) {
            if (submesh->useSharedVertices) {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(
                    Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(
                    posElem->getSource());

            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(
                Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //Ogre::Real* pReal;
            float* pReal;

            for (size_t j = 0; j < vertex_data->vertexCount;
                ++j, vertex += vbuf->getVertexSize()) {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = (orient * (pt * scale))
                    + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType()
            == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(
            Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset =
            (submesh->useSharedVertices) ? shared_offset : current_offset;

        if (use32bitindexes) {
            for (size_t k = 0; k < numTris * 3; ++k) {
                indices[index_offset++] = pLong[k]
                    + static_cast<unsigned long>(offset);
            }
        } else {
            for (size_t k = 0; k < numTris * 3; ++k) {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k])
                    + static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

////////////////////////////////////////////////////////////////////////////////
int
getResourcePath(const Ogre::String& resourceGroup,
                const Ogre::String& resourceName,
                Ogre::String &resourcePath)
{
    std::string sNameFullPath;

    /* First find file absolute path */
    Ogre::ResourceGroupManager& resGM =
        Ogre::ResourceGroupManager::getSingleton();
    Ogre::FileInfoListPtr files = resGM.findResourceFileInfo(resourceGroup,
        resourceName);

    if (files.isNull()) {
        debug("%s", "Recurso no encontrado.\n");
        return -1;

    } else {
        Ogre::FileInfoList::iterator it;
        for (it = files->begin(); it < files->end(); it++) {
            /* Compose file absolute path */
            sNameFullPath.append(it->archive->getName() + "/" + resourceName);
            if (fileExists(sNameFullPath)) {
                break;
            } else {
                sNameFullPath.clear();
            }
        }
        /* Found? */
        if (it == files->end() || sNameFullPath.size() <= 0) {
            debug("%s", "Recurso no encontrado.\n");
            return -1;
        }
    }

    resourcePath = sNameFullPath;

    return 0;
}

}

} /* namespace game */
