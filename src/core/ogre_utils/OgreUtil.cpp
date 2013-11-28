/*
 * OgreUtil.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: agustin
 */

#include "OgreUtil.h"

#include <OgreSubMesh.h>
#include <OgreMesh.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgreTexture.h>
#include <OgreTextureUnitState.h>
#include <OgreTextureManager.h>
#include <OgrePass.h>

#include <set>
#include <bitset>
#include <cmath>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>
#include <math/FloatComp.h>


// Helper methods
//
namespace {

// @brief Check if two vertices are equal or not using an epsilon as the
//        maximum squared distance between these two vertices to be considered
//        them as equals
//
inline bool
areVerticesEqual(const Ogre::Vector3& v1,
                 const Ogre::Vector3& v2,
                 float epsilon = FLOAT_COMP_THRESHOLD)
{
    return (v1.squaredDistance(v2) < epsilon);
}

}

namespace core {
namespace OgreUtil{

////////////////////////////////////////////////////////////////////////////////
bool
getMeshInformation(const Ogre::Mesh* const mesh,
                   core::size_t &vCount,
                   Ogre::Vector3* vertices,
                   core::size_t &iCount,
                   unsigned long* indices,
                   const Ogre::Matrix4& transform)
{
    const core::size_t maxVertices = vCount;
    const core::size_t maxIndices = iCount;
    bool added_shared = false;
    core::size_t current_offset = 0;
    core::size_t shared_offset = 0;
    core::size_t next_offset = 0;
    core::size_t index_offset = 0;

    vCount = iCount = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0, size = mesh->getNumSubMeshes(); i < size; ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices) {
            if( !added_shared ) {
                vCount += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        } else {
            vCount += submesh->vertexData->vertexCount;
        }
        // Add the indices
        iCount += submesh->indexData->indexCount;
    }

    // check if we have enough space
    if (vCount > maxVertices || iCount > maxIndices) {
        debugERROR("We cannot read all the vertices or indices on the current"
            " buffers: iCount: %zu | %zu,\t vCount: %zu | %zu\n",
            iCount, maxIndices, vCount, maxVertices);
        return false;
    }

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0, size = mesh->getNumSubMeshes(); i < size; ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ?
            mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) ||
            (submesh->useSharedVertices && !added_shared)) {
            if(submesh->useSharedVertices) {
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

            for( core::size_t j = 0, vertCount = vertex_data->vertexCount;
                j < vertCount; ++j, vertex += vbuf->getVertexSize()) {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = transform * pt;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        core::size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        core::size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes ) {
            for ( size_t k = 0; k < numTris*3; ++k) {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        } else {
            for ( size_t k = 0; k < numTris*3; ++k) {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                                          static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
removeDuplicated(Ogre::Vector3* vertices,
                 core::size_t& vCount,
                 unsigned long* indices,
                 core::size_t iCount)
{
    ASSERT(vertices);
    ASSERT(indices);
    if (vCount == 0) {
        debugWARNING("You are trying to remove duplicated from a empty list?\n");
        return;
    }

    // We will do an slow algorithm here but easy to implement.
    //
    ASSERT(vCount < 10000 && "To much vertices?\n");
    core::StackVector<unsigned int, 10000> modifiedIndices;
    core::StackVector<Ogre::Vector3, 10000> verts;
    std::set<unsigned int> toRemove;
    modifiedIndices.resize(vCount);
    for (unsigned int i = 0; i < vCount; ++i) {
        modifiedIndices[i] = i;
    }

    // copy the verts
    for (unsigned int i = 0; i < vCount; ++i) {
        verts.push_back(vertices[i]);
    }

    // now we will remove all the vertices we consider equal
    for (unsigned int i = 0; i < vCount; ++i) {
        for (unsigned int j = i+1; j < vCount; ++j) {
            if (areVerticesEqual(verts[i], verts[j])) {
                // we will remove this position and let the index to be pointing
                // to the ith element since is the same than this one (jth).
                modifiedIndices[j] = std::min(modifiedIndices[i], modifiedIndices[j]);
                if (modifiedIndices[j] < modifiedIndices[i]) {
                    modifiedIndices[i] = modifiedIndices[j];
                    toRemove.insert(i);
                }
                toRemove.insert(j);
            }
        }
    }

    // now we will remove each vertex from the array and update for each case
    // the indices
    unsigned int offset = 0;
    for (std::set<unsigned int>::iterator it = toRemove.begin(), endIt = toRemove.end();
            it != endIt; ++it) {
        const unsigned int currentIndex = *it - offset;
        verts.erase(currentIndex);

        // now update the modified indices for each case
        unsigned int begin = *it;
        for (; begin < modifiedIndices.size(); ++begin) {
            if (modifiedIndices[begin] > currentIndex) {
                modifiedIndices[begin] -= 1;
            }
        }
        ++offset;
    }

    // now we will remove all the vertices that are duplicated, to do this we
    unsigned int newVCount = vCount - toRemove.size();
#ifdef DEBUG
    // we will ensure that all the vertices are pointing to the first newVCount
    // vertices
    //
    for (unsigned int i = 0; i < vCount; ++i) {
        if (modifiedIndices[i] >= newVCount) {
            debugRED("modifiedIndices[%d]: %d, newVCount: %d\n", i, modifiedIndices[i], newVCount);
        }
        ASSERT(modifiedIndices[i] < newVCount);
    }
#endif
    // we will update the triangles also, to point to the unique indices
    for (unsigned int i = 0; i < iCount; ++i) {
        ASSERT(indices[i] < vCount);
        ASSERT(modifiedIndices[indices[i]] < newVCount);
        indices[i] = modifiedIndices[indices[i]];
    }

    // now we update the indices
    vCount = newVCount;
    // copy again the memory into vertices
    std::memcpy(vertices, verts.begin(), sizeof(Ogre::Vector3) * vCount);
}

////////////////////////////////////////////////////////////////////////////////
bool
getContourVertices(const Ogre::Mesh* const mesh,
                   core::size_t &vCount,
                   Ogre::Vector3* vertices,
                   const Ogre::Vector3& translate)
{
    ASSERT(mesh);
    const core::size_t realVertSize = vCount;
    unsigned long indices[2048];
    core::size_t iCount = 2048;

    Ogre::Matrix4 transform(Ogre::Matrix4::IDENTITY);
    transform.setTrans(translate);

    if (!getMeshInformation(mesh, vCount, vertices, iCount, indices, transform)) {
        debugERROR("Error when trying to get the information of mesh %s\n",
            mesh->getName().c_str());
        return false;
    }

    return getContourVertices(vertices, vCount, indices, iCount);
}

////////////////////////////////////////////////////////////////////////////////
bool
getContourVertices(Ogre::Vector3* vertices,
                   core::size_t &vCount,
                   unsigned long* indices,
                   core::size_t iCount)
{
    ASSERT(vertices);
    ASSERT(indices);

    if (vCount < 3 || iCount < 3) {
        debugWARNING("There is nothing to do with %zu vertices and %zu indices\n",
            vCount, iCount);
        return false;
    }

    // now we will remove the duplicated vertices
    removeDuplicated(vertices, vCount, indices, iCount);

    // now we will have to detect all the edges and triangles and mark the edges
    // as interior or exterior edges.
    struct Edge {
        unsigned int v1;
        unsigned int v2;
        mutable unsigned int numTris;

        Edge(unsigned int indexVert1, unsigned int indexVert2) :
            v1((indexVert1 < indexVert2) ? indexVert1 : indexVert2)
        ,   v2((indexVert1 < indexVert2) ? indexVert2 : indexVert1)
        ,   numTris(1)
        {}

        bool operator==(const Edge& o) const { return v1 == o.v1 && v2 == o.v2; }
        bool operator<(const Edge& o) const { return (v1 < o.v1) || (v2 < o.v2);}
    };

    // note here that we can increment the performance using a hash function
    // instead of a map, where the hash value is for example (v1 << 16) | v2
    // but this kind of methods should be called offline
    //

    // the Algorithm:
    // We will insert all the possible edges we have, if the edge already exists
    // this means that is shared for more than one triangle. If not, then is an
    // exterior edge.
    // After inserting all the edges we will remove all the interior ones and
    // let only the exterior's. We need to check also that from any edge we can
    // visit all the others, if not, then there are more than one contour and
    // we don't support that (now)
    //
    if ((iCount % 3) != 0) {
        debugERROR("Invalid indices count? we haven't triangles?!: %zu\n", iCount);
        return false;
    }

    // 1) Create the edges
    std::list<Edge> edges;
    for (unsigned int i = 0; i < iCount; i+=3) {
        // add the three associated edges for this triangle
        for (unsigned int j = 0; j < 3; ++j) {
            Edge e(indices[j+i], indices[i+((j+1)%3)]);
            auto it = std::find(edges.begin(), edges.end(), e);
            if (it == edges.end()) {
                // insert it
                edges.push_back(e);
            } else {
                // we need to increment the number of triangles associated to this
                // edge
                (*it).numTris += 1;
            }
        }
    }

    // Remove all the interior edges
    edges.remove_if([](const Edge& e) {return e.numTris > 1;});

    // now we have only exterior edges.... so now we have to check that going
    // from one (any) edge we can visit all the others.
    // Also we must ensure that all the vertices are associated to 2 edges only
    //
    ASSERT(vCount < 1024 && "We are not supporting more than 1024 unique vertices"
        " probably we need to increase this value");
    core::StackVector<core::StackVector<const Edge*, 2>, 1024> vertToEdgeMap;
    std::bitset<1024> vertsVisited;
    vertToEdgeMap.resize(vCount);

    for (auto it = edges.begin(); it != edges.end(); ++it) {
        vertToEdgeMap[it->v1].push_back(&(*it));
        vertToEdgeMap[it->v2].push_back(&(*it));
    }

    // now that we have the map, we will
    vertsVisited.reset();
    core::StackVector<Ogre::Vector3, 1024> result;
    ASSERT(edges.begin() != edges.end());
    const Edge* current = &(*edges.begin());
    unsigned int toVisit = current->v2;
    while (!vertsVisited[toVisit]) {
        vertsVisited[toVisit] = 1;

        // push the vertex
        result.push_back(vertices[toVisit]);

        // get the next edge
        core::StackVector<const Edge*, 2>& vem = vertToEdgeMap[toVisit];
        ASSERT(vem.size() == 2);
        ASSERT(vem[0] == current || vem[1] == current);
        current = (vem[0] == current) ? vem[1] : vem[0];
        toVisit = (vertsVisited[current->v1]) ? current->v2 : current->v1;
    }

    if (result.size() != edges.size()) {
        debugERROR("We have some interior contours? This means that the mesh is "
            "composed for not connected triangle lists, exterior edges: %zu and"
            " vertices analyzed: %d\n", edges.size(), result.size());
        return false;
    }

    // now we should check if they are CW or CCW order... and sort..
    ASSERT(result.size() > 2);
    const float crossProdXY = result[0].x * result[1].y - result[0].y * result[1].x;
    if (crossProdXY < 0) {
        // CW, we need to revert the result
        for (int i = result.size()-1, index = 0; i >= 0; --i, ++index) {
            vertices[index] = result[i];
        }
    } else {
        // is CCW, we can just copy the mem
        std::memcpy(vertices, result.begin(), sizeof(Ogre::Vector3) * result.size());
    }
    vCount = result.size();

    // everything goes fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
Ogre::Entity*
loadEntity(const Ogre::String& name, Ogre::SceneManager* sceneMngr)
{
    ASSERT(sceneMngr);

    Ogre::Entity* ent = 0;
    try {
        ent = sceneMngr->createEntity(name);
    } catch (...) {
        debugERROR("Error loading mesh %s, not found\n", name.c_str());
    }
    return ent;
}

////////////////////////////////////////////////////////////////////////////////
bool
getTextureSize(Ogre::Material* mat, core::Vector2& sizes)
{
    if (mat == 0 ||
        mat->getTechnique(0) == 0 ||
        mat->getTechnique(0)->getPass(0) == 0 ||
        mat->getTechnique(0)->getPass(0)->getTextureUnitState(0) == 0) {
        return false;
    }
    const Ogre::TextureUnitState* unit =
        mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(
        unit->getTextureName());
    if (texture.isNull()) {
        return false;
    }

    sizes.x = texture->getWidth();
    sizes.y = texture->getHeight();
    return true;
}

} /* namespace OgreUtil */
} /* namespace core */
