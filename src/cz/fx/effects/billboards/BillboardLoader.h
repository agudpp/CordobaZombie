/*
 * BillboardLoader.h
 *
 *  Created on: Oct 8, 2013
 *      Author: agustin
 */

#ifndef BILLBOARDLOADER_H_
#define BILLBOARDLOADER_H_



// forward
//
class TiXmlElement;

namespace cz {

/*
 *         unsigned int numBillboards;
        float width;
        float height;
        Ogre::Vector3 upVector;
        Ogre::Vector3 direction;
        Ogre::String materialName;
 */

// @brief Class used to load a BillboardGroupHandler and the associated stacks
//        from an xml.
//
// The xml:
//  <BillboardGroupInfo>
//      <BillboardStack id="1"
//                      numBillboards="10"
//                      width="10"
//                      height="10"
//                      matName="bloodBillboard1">
//      </BillboardStack>
//      <BillboardStack id="2"
//                      numBillboards="10"
//                      width="10"
//                      height="10"
//                      matName="bloodBillboard2">
//      </BillboardStack>
//      <Group stackID="1"  u0="0" u1="0.25" v0="0" v1="1">
//      <Group stackID="1"  u0="0" u1="0.25" v0="0" v1="1">
//      <Group stackID="2"  u0="0" u1="0.25" v0="0" v1="1">
//  </BillboardGroupInfo>

// forward
//
class BillboardGroupHandler;

namespace BillboardLoader
{

// @brief Load and configure a BillboardGroupHandler.
// @param xml       The xml used to load an configure the handler.
// @param handler   The handler to be configured
// @return true on success | false otherwise
// @note EVERY BillboardStack will be allocated and put into the GroupHandler
//       so you must remove (free) the memory before removing the
//       BillboardGroupHandler.
//
bool
loadAndConfigure(const TiXmlElement* xml, BillboardGroupHandler& handler);



};

} /* namespace cz */
#endif /* BILLBOARDLOADER_H_ */
